//
// Console.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Console.h"

#include "core/Log.h"

using namespace iqlogger::processor::V8;

void Console::log(const std::string& message) {
  INFO("JS LOG: " << message);
}

void Console::error(const std::string& message) {
  ERROR("JS ERROR: " << message);
}