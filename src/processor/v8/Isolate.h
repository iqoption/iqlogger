//
// Isolate.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "V8.h"

namespace iqlogger::processor::V8 {

class Isolate
{
public:
  explicit Isolate(unsigned short processorIndex);
  ~Isolate();

  v8::Isolate* getIsolate() const;

  void collectStats() const;

private:

  unsigned short m_processorIndex;

  v8::Isolate::CreateParams m_create_params;
  v8::Isolate* m_isolate;
  mutable std::atomic<uint64_t> m_heap_size;
};

}  // namespace iqlogger::processor
