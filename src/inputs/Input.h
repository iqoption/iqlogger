//
// Input.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "InputInterface.h"
#include "MainMessageQueue.h"
#include "metrics/MetricsController.h"
#include "metrics/Metrics.h"
#include "Module.h"

namespace iqlogger::inputs {

template<class T>
class Input : public InputInterface
{
public:
  explicit Input(const config::SourceConfig& sourceConfig) :
      InputInterface(sourceConfig),
      m_inputQueuePtr{std::make_shared<RecordQueue<T>>("inputs." + m_name + ".queue", sourceConfig.max_queue_size,
                                                       sourceConfig.overflow_strategy)},
      m_receive_counter(0) {
    TRACE("Input::Input()");
  }

  unsigned long importMessages() const override {
    auto mainMessageQueuePtr = MainMessageQueue::getInstance();

    auto& record_input_buffer = getRecordInputBuffer();
    auto& message_output_buffer = getMessageOutputBuffer();

    std::size_t count = m_inputQueuePtr->try_dequeue_bulk(record_input_buffer.begin()), j = 0, k = 0;

    if (count) {
      TRACE("Import " << count << " messages from " << m_name);

      for (; j < count; ++j) {
        try {
          message_output_buffer[k] = record_input_buffer[j]->exportMessage();
          ++k;
        } catch (const Exception& e) {
          WARNING("Error construct message: " << e.what());
        }
      }

      if (k) {
        m_receive_counter += k;

#ifdef IQLOGGER_WITH_PROCESSOR
        if (hasProcessor()) {
          TRACE("Send " << count << " messages to processor " << m_name);

          if (!process(std::make_move_iterator(message_output_buffer.begin()), k)) {
            ERROR("Processor queue is full... Dropping...");
          }
        } else {
#endif
          if (!mainMessageQueuePtr->enqueue_bulk(std::make_move_iterator(message_output_buffer.begin()), k)) {
            ERROR("Main queue is full... Dropping...");
          }
#ifdef IQLOGGER_WITH_PROCESSOR
        }
#endif
      }
    }

    return k;
  }

protected:

  void initImpl(std::any) override {
    // @TODO
  }

  void startImpl() override {

    INFO("Register metrics for input " << m_name);
    metrics::MetricsController::getInstance()->registerMetric("inputs." + m_name + ".receive_total",
                                                              [this]() { return m_receive_counter.load(); });
  }

  void stopImpl() override {
    // @TODO
  }

  void startModule() override {
    INFO("Start module for input " << m_name);
    Module<Input<T>>::getInstance()->start();
  }

  void stopModule() override {
    INFO("Stop module for input " << m_name);
    Module<Input<T>>::getInstance()->stop();
  }

  static std::array<RecordPtr<T>, max_queue_bulk_size>& getRecordInputBuffer() {
    static thread_local std::array<RecordPtr<T>, max_queue_bulk_size> record_input_buffer;
    return record_input_buffer;
  }

  static std::array<UniqueMessagePtr, max_queue_bulk_size>& getMessageOutputBuffer() {
    static thread_local std::array<UniqueMessagePtr, max_queue_bulk_size> message_output_buffer;
    return message_output_buffer;
  }

  RecordQueuePtr<T> m_inputQueuePtr;
  mutable metrics::atomic_metric_t m_receive_counter;

};
}  // namespace iqlogger::inputs
