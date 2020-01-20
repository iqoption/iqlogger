//
// Isolate.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Isolate.h"

#include "core/Log.h"
#include "metrics/MetricsController.h"

using namespace iqlogger;
using namespace iqlogger::processor::V8;

Isolate::Isolate(unsigned short processorIndex) :
    m_processorIndex(processorIndex),
    m_isolate([& create_params = m_create_params]() {
      create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
      return v8::Isolate::New(create_params);
    }()) {
  TRACE("Isolate::Isolate()");

  metrics::MetricsController::getInstance()->registerMetric(
      "processor." + std::to_string(m_processorIndex) + ".heap_size",
      [& heap_size = m_heap_size]() { return heap_size.load(); });
}

Isolate::~Isolate() {
  TRACE("Isolate::~Isolate()");
  m_isolate->Dispose();
  delete m_create_params.array_buffer_allocator;
}

v8::Isolate* Isolate::getIsolate() const {
  return m_isolate;
}

void Isolate::collectStats() const {
  TRACE("Isolate::collectStats()");
  v8::HeapStatistics stats;
  m_isolate->GetHeapStatistics(&stats);
  m_heap_size.store(stats.total_heap_size());
}