//
// ProcessorMachine.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "ProcessorMachine.h"

#include "MainMessageQueue.h"
#include "ProcessorException.h"
#include "config/ConfigManager.h"
#include "core/Log.h"
#include "v8/Processor.h"

using namespace iqlogger::processor;
using namespace iqlogger::processor::V8;

ProcessorMachine::ProcessorMachine() :
    m_thread_num{config::ConfigManager::getInstance()->getConfig()->processorConfig.threads},
    m_processQueue("processor.queue") {
  TRACE("ProcessorMachine::ProcessorMachine()");
  DEBUG("ProcessorMachine init done");
}

void ProcessorMachine::initImpl(std::any) {}

void ProcessorMachine::startImpl() {
  TRACE("ProcessorMachine::start()");

  DEBUG("Initialize V8 Engine: " << Engine::getInstance());

  DEBUG("Retrieve scripts from config...");

  for (size_t i = 0; i < m_thread_num; ++i) {
    m_threads.emplace_back(std::thread([i, this]() {
      try {
        ScriptMap scriptMap;

        // @FIXME
        for (ProcessorScriptIndex index = 0; index < m_scriptSources.size(); ++index) {
          scriptMap.emplace(index, m_scriptSources[index]);
        }

        Processor processor(i, std::move(scriptMap));
        processor.init({});
        processor.start();

        std::size_t count, j, k;

        std::array<ProcessorRecordPtr, max_queue_bulk_size> record_input_buffer;
        std::array<UniqueMessagePtr, max_queue_bulk_size> message_output_buffer;

        while (isRunning()) {
          count = m_processQueue.try_dequeue_bulk(record_input_buffer.begin(), max_queue_bulk_size);

          if (count) {
            TRACE("Import " << count << " messages in processor thread...");
            for (j = k = 0; j < count; ++j) {
              try {
                if (auto result = processor.process(std::move(record_input_buffer[j])); result) {
                  message_output_buffer[k] = std::move(result);
                  ++k;
                } else {
                  DEBUG("Ignore message in processor");
                }
              } catch (const Exception& e) {
                WARNING("Error process message from: " << e.what());
              }
            }

            if (k) {
              if (!MainMessageQueue::getInstance()->enqueue_bulk(std::make_move_iterator(message_output_buffer.begin()),
                                                                 k)) {
                ERROR("Main queue is full... Dropping...");
              }
            }

          } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }

          processor.collectStats();
        }

        processor.stop();

      } catch (const std::exception& e) {
        CRITICAL("Exception " << e.what());
        stop();
      }
    }));
  }

  TRACE("ProcessorMachine::start() <-");
}

void ProcessorMachine::stopImpl() {
  TRACE("ProcessorMachine::stop()");

  DEBUG("Join processor threads... ");
  for (auto& t : m_threads) {
    if (t.joinable())
      t.join();
  }
}

ProcessorScriptIndex ProcessorMachine::addProcessor(const std::string& script_source) {
  TRACE("ProcessorMachine::addProcessor()");
  INFO("Add ProcessorMachine script: " << script_source);
  ProcessorScriptIndex index = std::distance(m_scriptSources.begin(), m_scriptSources.push_back(script_source));
  DEBUG("ProcessorMachine index is: " << index);
  return index;
}

bool ProcessorMachine::checkScript(const std::string& script_source) const {
  DEBUG("Initialize V8 Engine: " << Engine::getInstance());

  try {
    ScriptMap scriptMap;
    scriptMap.emplace(0, script_source);

    Processor processor(0, std::move(scriptMap));
  } catch (const std::exception& e) {
    ERROR("Exception due script check: " << e.what());
    return false;
  }

  DEBUG("Processor script checked (compiled) succeed");
  return true;
}
