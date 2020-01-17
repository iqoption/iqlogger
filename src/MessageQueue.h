//
// MessageQueue.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "concurrentqueue/concurrentqueue.h"

#include "metrics/MetricsController.h"
#include "core/Log.h"
#include "config/core/QueueOverflowStrategy.h"
#include "Message.h"

namespace iqlogger {

    static constexpr size_t max_queue_bulk_size = 10'000;

    template<typename T>
    class MessageQueue {

        using QueueT = moodycamel::ConcurrentQueue<T>;
        using QueueTPtr = std::unique_ptr<QueueT>;

        std::string                     m_name;
        QueueTPtr                       m_messageQueuePtr;
        size_t                          m_max_queue_size    = 10'000;
        config::QueueOverflowStrategy   m_overflow_stategy  = config::QueueOverflowStrategy::THROTTLE;
        std::atomic<size_t>             m_dropped           {0};
        std::atomic<size_t>             m_throttled         {0};

    public:

        MessageQueue(const MessageQueue&) = delete;
        MessageQueue& operator =(const MessageQueue&) = delete;

        MessageQueue(MessageQueue&&) = default;

        ~MessageQueue() = default;

        explicit MessageQueue(const std::string& name) :
            m_name (name),
            m_messageQueuePtr {std::make_unique<QueueT>()}
        {
            registerMetrics();
        }

        explicit MessageQueue(const std::string& name, size_t max_queue_size, config::QueueOverflowStrategy overflow_stategy) :
            m_name (name),
            m_messageQueuePtr {std::make_unique<QueueT>()},
            m_max_queue_size (max_queue_size),
            m_overflow_stategy (overflow_stategy)
        {
            registerMetrics();
        }

        template<typename It>
        bool enqueue_bulk(It itemFirst, size_t count)
        {
            if(m_messageQueuePtr->size_approx() >= m_max_queue_size)
            {
                if(m_overflow_stategy == config::QueueOverflowStrategy::DROP)
                {
                    m_dropped += count;
                    ERROR ("Queue " << m_name << " DROP " << count << " messages!!!");
                    return false;
                }
                else if(m_overflow_stategy == config::QueueOverflowStrategy::THROTTLE)
                {
                    m_throttled += count;

                    while (m_messageQueuePtr->size_approx() >= m_max_queue_size)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                }
                else
                {
                    ERROR ("Queue " << m_name << " DROP " << count << " messages with undefined strategy!!!");
                    return false;
                }
            }

            return m_messageQueuePtr->enqueue_bulk(itemFirst, count);
        }

        // @TODO Универсальная ссылка
        bool enqueue(T&& item)
        {
            // @TODO Убрать дублирование кода
            if(m_messageQueuePtr->size_approx() >= m_max_queue_size)
            {
                if(m_overflow_stategy == config::QueueOverflowStrategy::DROP)
                {
                    m_dropped++;
                    ERROR ("Queue " << m_name << " DROP 1 message!!!");
                    return false;
                }
                else if(m_overflow_stategy == config::QueueOverflowStrategy::THROTTLE)
                {
                    m_throttled ++;

                    while (m_messageQueuePtr->size_approx() >= m_max_queue_size)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                }
                else
                {
                    ERROR ("Queue " << m_name << " DROP 1 message with undefined strategy!!!");
                    return false;
                }
            }

            return m_messageQueuePtr->enqueue(std::move(item));
        }

        bool try_dequeue(T& item)
        {
            return m_messageQueuePtr->try_dequeue(item);
        }

        template<typename It>
        size_t try_dequeue_bulk(It itemFirst, size_t max = max_queue_bulk_size)
        {
            return m_messageQueuePtr->try_dequeue_bulk(itemFirst, max);
        }

    private:

        void registerMetrics() const
        {
            INFO ("Register metrics for queue " << m_name);

            metrics::MetricsController::getInstance()->registerMetric(m_name + ".length", [this]() {
                return m_messageQueuePtr->size_approx();
            });

            metrics::MetricsController::getInstance()->registerMetric(m_name + ".dropped", [this]() {
                return m_dropped.load();
            });

            metrics::MetricsController::getInstance()->registerMetric(m_name + ".throttled", [this]() {
                return m_throttled.load();
            });
        }
    };

    using UniqueMessageQueue = MessageQueue<UniqueMessagePtr>;
    using UniqueMessageQueuePtr = std::shared_ptr<UniqueMessageQueue>;
}

