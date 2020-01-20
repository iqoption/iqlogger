//
// ConsoleWrapper.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "V8.h"

namespace iqlogger::processor::V8 {

class ConsoleWrapper
{
public:

  static void log(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void error(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}  // namespace iqlogger::processor::V8
