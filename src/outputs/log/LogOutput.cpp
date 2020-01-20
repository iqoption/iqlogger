//
// LogOutput.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "LogOutput.h"

#include "outputs/Record.h"
#include "outputs/log/Log.h"

using namespace iqlogger::outputs;
using namespace iqlogger::outputs::log;

LogOutput::LogOutput(config::DestinationConfig destinationConfig,
                     metrics::atomic_metric_t& total_outputs_send_counter) :
    Output::Output(destinationConfig) {
  m_outputThreadCallback = [this, &total_outputs_send_counter]() {
    constexpr size_t queue_bulk = 1000;

    std::array<RecordPtr<Log>, queue_bulk> bulk;
    std::size_t count;

    while (isRunning()) {
      count = m_outputQueuePtr->try_dequeue_bulk(bulk.begin(), queue_bulk);

      if (count) {
        for (size_t j = 0; j < count; ++j) {
          auto str = bulk[j]->exportRecord();
          INFO(str);
        }

        m_send_counter += count;
        total_outputs_send_counter += count;
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }
  };
}

void LogOutput::startImpl() {
  Output<Log>::startImpl();

  for (size_t i = 0; i < m_thread_num; ++i) {
    m_threads.emplace_back(std::thread(m_outputThreadCallback));
  }
}

void LogOutput::stopImpl() {
  Output<Log>::stop();

  for (auto& t : m_threads) {
    if (t.joinable())
      t.join();
  }
}
