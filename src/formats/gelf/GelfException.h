//
// GelfException.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <iostream>

#include "Exception.h"

namespace iqlogger::formats::gelf {

class GelfException : public Exception
{
  using Exception::Exception;
};

class GelfUncompressError : public GelfException
{
  using GelfException::GelfException;
};

class GelfParseError : public GelfException
{
  using GelfException::GelfException;
};
}  // namespace iqlogger::formats::gelf
