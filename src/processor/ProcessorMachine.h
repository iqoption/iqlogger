//
// ProcessorMachine.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <concurrentqueue/concurrentqueue.h>
#include <memory>
#include <tbb/concurrent_vector.h>
#include <vector>

#include "MessageQueue.h"
#include "ProcessorTypes.h"
#include "core/Log.h"
#include "core/Singleton.h"
#include "core/TaskInterface.h"

namespace iqlogger::processor {

class ProcessorMachine : public Singleton<ProcessorMachine>, public TaskInterface
{
  friend class Singleton<ProcessorMachine>;

  using InternalScriptMap = tbb::concurrent_vector<std::string>;
  using ProcessorRecordQueue = MessageQueue<ProcessorRecordPtr>;
  using ProcessorRecordPtrBuffer = std::array<ProcessorRecordPtr, max_queue_bulk_size>;

public:
  ~ProcessorMachine() = default;

  bool checkScript(const std::string& script_source) const;

  ProcessorScriptIndex addProcessor(const std::string& script_source);

  template<typename It>
  bool process_bulk(It itemFirst, size_t count, ProcessorScriptIndex processorScriptIndex) {
    auto& record_buffer = getProcessorRecordPtrBuffer();

    for (size_t i = 0; i < count; i++) {
      record_buffer[i] =
          std::make_unique<ProcessorRecord>(std::make_pair(std::move(*(itemFirst + i)), processorScriptIndex));
    }

    return m_processQueue.enqueue_bulk(std::make_move_iterator(record_buffer.begin()), count);
  }

  bool process(UniqueMessagePtr&& item, ProcessorScriptIndex processorScriptIndex) {
    auto record = std::make_unique<ProcessorRecord>(std::move(item), processorScriptIndex);
    return m_processQueue.enqueue(std::move(record));
  }

protected:
  void initImpl(std::any) override;
  void startImpl() override;
  void stopImpl() override;

private:
  ProcessorMachine();

  static ProcessorRecordPtrBuffer& getProcessorRecordPtrBuffer() {
    static thread_local ProcessorRecordPtrBuffer record_buffer;
    return record_buffer;
  }

private:
  size_t m_thread_num;
  std::vector<std::thread> m_threads;
  //  std::vector<utils::atomic::aligned_atomic_t<uint64_t>> m_heap_sizes;

  InternalScriptMap m_scriptSources;

  ProcessorRecordQueue m_processQueue;
};

}  // namespace iqlogger::processor
