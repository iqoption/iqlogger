//
// JournalInput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "inputs/IOInput.h"
#include "JournalInput.h"
#include "JournalReader.h"

namespace iqlogger::inputs::journal {

    class JournalInput : public IOInput<Journal> {

        JournalReaderPtr m_readerPtr;

    public:

        explicit JournalInput(const config::SourceConfig& sourceConfig);
        virtual ~JournalInput();

        virtual void start() override;
        virtual void stop() override;
    };
}


