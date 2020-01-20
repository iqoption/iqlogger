//
// ProcessorException.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "Exception.h"

namespace iqlogger::processor {

class ProcessorException : public Exception
{
  using Exception::Exception;
};
}  // namespace iqlogger::processor
