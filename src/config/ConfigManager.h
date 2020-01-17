//
// ConfigManager.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <filesystem>

#include "core/Singleton.h"
#include "config/Config.h"

namespace iqlogger::config {

    class ConfigManager : public Singleton<ConfigManager> {

        friend class Singleton<ConfigManager>;

        std::filesystem::path m_configFilename;
        std::string       m_hostname;

        ConfigPtr m_configPtr;

        ConfigManager();
        bool loadLogger();

    public:

        bool load(std::string const& filename, bool checkOnly = false);
        bool reload(bool checkOnly = false);

        ConfigPtr getConfig();

        std::string getHostName() const noexcept;
        std::filesystem::path getConfigPath() const;
    };
}


