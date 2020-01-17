//
// LoggerConfig.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "config/ConfigInterface.h"
#include "core/Log.h"

namespace iqlogger::config {

    struct LoggerConfig : public ConfigInterface {

        std::string filename;
        std::string level;
        uint64_t    size        = 1024 * 1024 * 100;    // Default 100Mb
        size_t      count       = 5;                    // Default 5 files

        LoggerConfig() noexcept = default;
    };
}

namespace nlohmann {

    template <>
    struct adl_serializer<iqlogger::config::LoggerConfig>
    {
        static void from_json(const json& j, iqlogger::config::LoggerConfig& loggerConfig);
    };
}

