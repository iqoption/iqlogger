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

namespace iqlogger::inputs::gelf::udp {

    class GelfInput : public IOInput<Gelf> {

        std::shared_ptr<Server> m_serverPtr;
        ChunkQueuePtr m_chunkQueuePtr;
        std::thread m_chunkProcessThread;
        std::atomic<uint64_t> m_bad_chunks;
        unsigned short m_chunk_ttl;
        unsigned short m_port;

    public:

        explicit GelfInput(const config::SourceConfig& sourceConfig);

        virtual ~GelfInput();

        virtual void start() override;
        virtual void stop() override;
//        void restart() override;

    };
}


