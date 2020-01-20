//
// InputInterface.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>

#include "Exception.h"
#include "MessageQueue.h"
#include "Record.h"
#include "config/Config.h"
#include "core/TaskInterface.h"

#ifdef IQLOGGER_WITH_PROCESSOR
#include <processor/ProcessorMachine.h>
#endif

namespace iqlogger::inputs {

class InputInterface;
using InputPtr = std::unique_ptr<InputInterface>;

class InputInterface : public TaskInterface
{
public:
  explicit InputInterface(const config::SourceConfig& sourceConfig);
  virtual ~InputInterface() = default;

  static InputPtr instantiate(const config::SourceConfig& sourceConfig);

  virtual unsigned long importMessages() const = 0;

#ifdef IQLOGGER_WITH_PROCESSOR
  inline bool hasProcessor() const { return m_processorScriptIndex.has_value(); }

  template<typename It>
  inline bool process(It itemFirst, size_t count) const {
    if (hasProcessor()) {
      return iqlogger::processor::ProcessorMachine::getInstance()->process_bulk(itemFirst, count,
                                                                         m_processorScriptIndex.value());
    }

    return false;
  }
#endif

  virtual void startModule() = 0;
  virtual void stopModule() = 0;

private:
  template<config::InputType... Inputs>
  static InputPtr instantiateImpl(const config::SourceConfig& sourceConfig) {
    auto checker = utils::types::make_switch(
        [&]() -> InputPtr {
          std::stringstream oss;
          oss << "Unknown type " << sourceConfig.type;
          throw Exception(oss.str());
        },
        std::make_pair(Inputs, [&]() { return instantiateInput<Inputs>(sourceConfig); })...);
    return checker(sourceConfig.type);
  }

  template<config::InputType Input>
  static InputPtr instantiateInput(const config::SourceConfig& sourceConfig);

protected:
  std::string m_name;
  config::InputType m_type;

  size_t m_thread_num;
  std::vector<std::thread> m_threads;

#ifdef IQLOGGER_WITH_PROCESSOR
  std::optional<iqlogger::processor::ProcessorScriptIndex> m_processorScriptIndex;
#endif

};
}  // namespace iqlogger::inputs
