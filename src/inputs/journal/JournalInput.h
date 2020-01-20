//
// JournalInput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "JournalInput.h"
#include "JournalReader.h"
#include "inputs/IOInput.h"

namespace iqlogger::inputs::journal {

class JournalInput : public IOInput<Journal>
{
public:
  explicit JournalInput(const config::SourceConfig& sourceConfig);
  virtual ~JournalInput();

protected:

  void startImpl() override;
  void stopImpl() override;

private:
  JournalReaderPtr m_readerPtr;
};
}  // namespace iqlogger::inputs::journal
