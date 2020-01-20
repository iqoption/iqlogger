//
// Log.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <string>

namespace iqlogger::outputs::log {

class Log
{
public:
  using DataT = std::string;
  using MessageT = std::string;
  using DataExportT = std::string_view;
};
}  // namespace iqlogger::outputs::log
