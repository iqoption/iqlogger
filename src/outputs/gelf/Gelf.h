//
// Gelf.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <string>

#include "formats/gelf/GelfMessage.h"

namespace iqlogger::outputs::gelf {

class Gelf
{
public:
  using DataT = std::string;
  using MessageT = iqlogger::formats::gelf::GelfMessage;
  using DataExportT = std::string_view;
};
}  // namespace iqlogger::outputs::gelf
