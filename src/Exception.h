//
// Exception.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <stdexcept>

namespace iqlogger {

class Exception : public std::logic_error
{
  using std::logic_error::logic_error;
};
}  // namespace iqlogger
