//
// Engine.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Engine.h"

using namespace iqlogger::processor::V8;

Engine::Engine() {
  TRACE("Engine::Engine()");
  DEBUG("Initialize V8");

  v8::V8::InitializeICUDefaultLocation("./iqlogger");
  v8::V8::InitializeExternalStartupData("./iqlogger");
  m_platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(m_platform.get());
  v8::V8::Initialize();

  DEBUG("Initialize V8 complete");
}

Engine::~Engine() {
  TRACE("Engine::~Engine()");
  DEBUG("Destroy V8");

  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();

  DEBUG("Destroy V8 complete");
}

v8::Platform* Engine::getPlatformPtr() const {
  return m_platform.get();
}
