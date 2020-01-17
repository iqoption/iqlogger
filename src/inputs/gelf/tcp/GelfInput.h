//
// GelfInput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//
#pragma once

#include <inputs/IOInput.h>
#include "Server.h"

namespace iqlogger::inputs::gelf::tcp {

    class GelfInput : public IOInput<Gelf> {

        std::shared_ptr<Server> m_serverPtr;
        unsigned short m_port;

    public:

        explicit GelfInput(const config::SourceConfig& sourceConfig);
        virtual ~GelfInput();
        void start() override;
//        void restart() override;

    };

}


