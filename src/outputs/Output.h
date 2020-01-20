//
// Output.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "OutputInterface.h"
#include "Record.h"

namespace iqlogger::outputs {

template<class T>
class Output : public OutputInterface
{
  static std::array<RecordPtr<T>, max_queue_bulk_size>& getRecordPtrBuffer() {
    static thread_local std::array<RecordPtr<T>, max_queue_bulk_size> buffer;
    return buffer;
  }

public:
  explicit Output(config::DestinationConfig destinationConfig) :
      OutputInterface(destinationConfig),
      m_outputQueuePtr{std::make_shared<RecordQueue<T>>("outputs." + m_name + ".queue")},
      m_send_counter(0) {
    DEBUG("Initialize " << m_name << " Initial counter " << m_send_counter.load()
                        << " IsLockFree: " << m_send_counter.is_lock_free());
  }

  bool importMessages(message_ptr_buffer_t::const_iterator it, size_t count) const override {
    auto& buffer = getRecordPtrBuffer();

    for (size_t i = 0; i < count; ++i) {
      const Message& message = *(*(it + i));

      RecordPtr<T> recordPtr;

      std::visit(
          [&recordPtr](const auto& mess) {
            recordPtr = std::make_unique<Record<T>>(mess);
          },
          message);

      buffer[i] = std::move(recordPtr);
    }

    return m_outputQueuePtr->enqueue_bulk(std::make_move_iterator(buffer.begin()), count);
  }

protected:

  void startImpl() override {

    INFO("Register metrics for output " << m_name);

    metrics::MetricsController::getInstance()->registerMetric("outputs." + m_name + ".send_total",
                                                              [this]() { return m_send_counter.load(); });
  }

protected:
  RecordQueuePtr<T> m_outputQueuePtr;
  metrics::atomic_metric_t m_send_counter;
};
}  // namespace iqlogger::outputs
