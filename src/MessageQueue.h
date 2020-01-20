//
// MessageQueue.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <concurrentqueue/concurrentqueue.h>

#include "Message.h"
#include "config/core/QueueOverflowStrategy.h"
#include "core/Log.h"
#include "metrics/MetricsController.h"

namespace iqlogger {

static constexpr inline size_t max_queue_bulk_size = 10'000;

template<typename T>
class MessageQueue
{
  using QueueT = moodycamel::ConcurrentQueue<T>;
  using QueueTPtr = std::unique_ptr<QueueT>;
  using OverflowStrategy = config::QueueOverflowStrategy;

  std::string m_name;
  QueueTPtr m_messageQueuePtr;
  size_t m_max_queue_size = 10'000;
  OverflowStrategy::Type m_overflow_strategy = OverflowStrategy::Type::THROTTLE;
  std::atomic<size_t> m_dropped{0};
  std::atomic<size_t> m_throttled{0};

public:
  MessageQueue(const MessageQueue&) = delete;
  MessageQueue& operator=(const MessageQueue&) = delete;

  MessageQueue(MessageQueue&&) = delete;

  ~MessageQueue() = default;

  explicit MessageQueue(std::string name) : m_name(std::move(name)), m_messageQueuePtr{std::make_unique<QueueT>()} {
    registerMetrics();
  }

  explicit MessageQueue(std::string name, size_t max_queue_size, OverflowStrategy::Type overflow_strategy) :
      m_name(std::move(name)),
      m_messageQueuePtr{std::make_unique<QueueT>()},
      m_max_queue_size(max_queue_size),
      m_overflow_strategy(overflow_strategy) {
    registerMetrics();
  }

  template<typename It>
  bool enqueue_bulk(It itemFirst, size_t count) {
    if (m_messageQueuePtr->size_approx() >= m_max_queue_size) {
      if (m_overflow_strategy == config::QueueOverflowStrategy::Type::DROP) {
        m_dropped += count;
        ERROR("Queue " << m_name << " DROP " << count << " messages!!!");
        return false;
      } else if (m_overflow_strategy == config::QueueOverflowStrategy::Type::THROTTLE) {
        m_throttled += count;

        while (m_messageQueuePtr->size_approx() >= m_max_queue_size) {
          std::this_thread::sleep_for(config::QueueOverflowStrategy::throttle_interval);
        }
      } else {
        ERROR("Queue " << m_name << " DROP " << count << " messages with undefined strategy!!!");
        return false;
      }
    }

    return m_messageQueuePtr->enqueue_bulk(itemFirst, count);
  }

  template<typename U, typename = std::enable_if_t<std::is_same_v<T, U>>>
  bool enqueue(U&& item) {
    if (m_messageQueuePtr->size_approx() >= m_max_queue_size) {
      if (m_overflow_strategy == config::QueueOverflowStrategy::Type::DROP) {
        m_dropped++;
        ERROR("Queue " << m_name << " DROP 1 message!!!");
        return false;
      } else if (m_overflow_strategy == config::QueueOverflowStrategy::Type::THROTTLE) {
        m_throttled++;

        while (m_messageQueuePtr->size_approx() >= m_max_queue_size) {
          std::this_thread::sleep_for(config::QueueOverflowStrategy::throttle_interval);
        }
      } else {
        ERROR("Queue " << m_name << " DROP 1 message with undefined strategy!!!");
        return false;
      }
    }

    return m_messageQueuePtr->enqueue(std::forward<U>(item));
  }

  bool try_dequeue(T& item) { return m_messageQueuePtr->try_dequeue(item); }

  template<typename It>
  size_t try_dequeue_bulk(It itemFirst, size_t max = max_queue_bulk_size) {
    return m_messageQueuePtr->try_dequeue_bulk(itemFirst, max);
  }

private:
  void registerMetrics() const {
    INFO("Register metrics for queue " << m_name);

    metrics::MetricsController::getInstance()->registerMetric(m_name + ".length",
                                                              [this]() { return m_messageQueuePtr->size_approx(); });

    metrics::MetricsController::getInstance()->registerMetric(m_name + ".dropped",
                                                              [this]() { return m_dropped.load(); });

    metrics::MetricsController::getInstance()->registerMetric(m_name + ".throttled",
                                                              [this]() { return m_throttled.load(); });
  }
};

using UniqueMessageQueue = MessageQueue<UniqueMessagePtr>;
using UniqueMessageQueuePtr = std::shared_ptr<UniqueMessageQueue>;
}  // namespace iqlogger
