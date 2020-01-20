//
// Header.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <string>

namespace iqlogger::stats::http {

struct Header {
  std::string name;
  std::string value;
};
}  // namespace iqlogger::stats::http
