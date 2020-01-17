//
// TailInput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <filesystem>

#include "inputs/Input.h"
#include "TailMonitor.h"
#include "Tail.h"

namespace iqlogger::inputs::tail {

    class TailInput : public Input<Tail> {

        enum class DelimiterType {
            NEWLINE   = 0,
            REGEX     = 1
        };

        constexpr static unsigned short default_read_timeout        = 10;
        constexpr static bool           default_follow_only_mode    = false;

        std::string m_path;

        DelimiterType m_delimiterType;

        unsigned short m_read_timeout;

        TailMonitorPtr m_monitorPtr;

        std::thread m_timerThread;

    public:

        explicit TailInput(const config::SourceConfig& sourceConfig);
        virtual ~TailInput();

        virtual void start() override;
        virtual void stop() override;

    };
}


