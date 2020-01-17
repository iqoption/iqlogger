//
// JournalInput.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "JournalInput.h"
#include "Journal.h"

using namespace iqlogger;
using namespace iqlogger::inputs::journal;

JournalInput::JournalInput(const config::SourceConfig& sourceConfig) :
        IOInput<Journal>::IOInput(sourceConfig)
{
    TRACE("JournalInput::JournalInput()");
    m_readerPtr = std::make_unique<JournalReader>(m_inputQueuePtr, m_io_service, sourceConfig.getParam<std::vector<std::string>>("units"));
}

JournalInput::~JournalInput()
{
    TRACE("JournalInput::~JournalInput()");
}

void JournalInput::start()
{
    TRACE("JournalInput::start()");
    IOInput::start();
}

void JournalInput::stop()
{
    TRACE("JournalInput::stop()");
    IOInput::stop();
}

