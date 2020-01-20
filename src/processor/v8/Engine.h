//
// Engine.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>

#include "V8.h"
#include "core/Log.h"
#include "core/Singleton.h"

namespace iqlogger::processor::V8 {

class Engine : public Singleton<Engine>
{
  friend class Singleton<Engine>;

  Engine();

  std::unique_ptr<v8::Platform> m_platform;

public:
  virtual ~Engine();
  v8::Platform* getPlatformPtr() const;
};

using EnginePtr = std::shared_ptr<Engine>;
}  // namespace iqlogger::processor::V8
