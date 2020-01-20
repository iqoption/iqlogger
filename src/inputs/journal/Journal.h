//
// Journal.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "formats/journal/JournalMessage.h"

namespace iqlogger::inputs::journal {

class Journal
{
public:
  using RecordDataT = iqlogger::formats::journal::JournalMessage;
  using MessageT = RecordDataT;
  using SourceT = MessageT::SourceT;
};
}  // namespace iqlogger::inputs::journal
