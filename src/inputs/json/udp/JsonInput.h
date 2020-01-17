//
// JsonInput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <inputs/IOInput.h>
#include "Server.h"

namespace iqlogger::inputs::json::udp {

    class JsonInput : public IOInput<Json> {

        std::shared_ptr<Server> m_serverPtr;
        unsigned short m_port;

    public:

        explicit JsonInput(const config::SourceConfig& sourceConfig);

        virtual ~JsonInput();

        virtual void start() override;
        virtual void stop() override;
//        void restart() override;

    };
}


