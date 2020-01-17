//
// StatsExportServer.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>

#include "core/TaskInterface.h"
#include "config/Config.h"

namespace iqlogger::stats {

    class StatsExportServer;
    using StatsExportServerPtr = std::unique_ptr<StatsExportServer>;

    class StatsExportServer : public TaskInterface
    {
    public:

        StatsExportServer() = default;

        StatsExportServer(const StatsExportServer&) = delete;
        StatsExportServer& operator=(const StatsExportServer&) = delete;

        static StatsExportServerPtr instantiate(config::StatsEntryConfig statsEntryConfig);
    };
}


