//
// Console.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "core/Singleton.h"

namespace iqlogger::processor::V8 {

class Console : public Singleton<Console>
{
  friend class Singleton<Console>;

  Console() = default;

public:
  ~Console() = default;

  void log(const std::string& message);
  void error(const std::string& message);
};

}  // namespace iqlogger::processor
