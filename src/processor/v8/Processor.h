//
// Processor.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>

#include "Isolate.h"
#include "IsolateScope.h"
#include "core/TaskInterface.h"

namespace iqlogger::processor::V8 {

class Processor : public TaskInterface
{
public:
  Processor(unsigned short processorIndex, ScriptMap scriptMap);
  ~Processor();

  UniqueMessagePtr process(ProcessorRecordPtr processorRecordPtr);
  void collectStats() const;

protected:
  void initImpl(std::any) override;
  void startImpl() override;
  void stopImpl() override;

private:

  unsigned short m_processorIndex;

  Isolate m_isolate;
  IsolateScope m_isolateScope;
};

}  // namespace iqlogger::processor::V8
