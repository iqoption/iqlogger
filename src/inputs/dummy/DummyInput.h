//
// DummyInput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <chrono>

#include "inputs/IOInput.h"
#include "DummyInput.h"

namespace iqlogger::inputs::dummy {

    class DummyInput : public Input<Dummy> {

        std::string                 m_dummy_text;
        std::chrono::milliseconds   m_interval;

        std::thread m_thread;

    public:

        explicit DummyInput(const config::SourceConfig& sourceConfig);
        virtual ~DummyInput();

        virtual void start() override;
        virtual void stop() override;
    };
}


