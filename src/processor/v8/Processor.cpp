//
// Processor.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Processor.h"

#include "Console.h"
#include "core/Log.h"

using namespace iqlogger;
using namespace iqlogger::processor;
using namespace iqlogger::processor::V8;

Processor::Processor(unsigned short processorIndex, ScriptMap scriptMap) :
    m_processorIndex(processorIndex),
    m_isolate(m_processorIndex),
    m_isolateScope(m_isolate.getIsolate(), std::move(scriptMap)) {
  DEBUG("Processor init done");
}

void Processor::initImpl(std::any) {}

void Processor::startImpl() {
  TRACE("Processor::start()");

  TRACE("Processor::start() <-");
}

void Processor::stopImpl() {
  TRACE("Processor::stop()");
}

UniqueMessagePtr Processor::process(ProcessorRecordPtr processorRecordPtr) {
  TRACE("Processor::process()");
  return m_isolateScope.process(std::move(processorRecordPtr));
}

void Processor::collectStats() const {
  m_isolate.collectStats();
}

Processor::~Processor() {
  TRACE("Processor::~Processor()");
}
