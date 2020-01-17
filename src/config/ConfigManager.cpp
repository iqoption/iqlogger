//
// ConfigManager.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include <fstream>
#include <unistd.h>
#include <limits.h>

#include "ConfigManager.h"

using namespace iqlogger::config;

ConfigManager::ConfigManager()
{
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    m_hostname = hostname;
}

bool ConfigManager::load(std::string const &filename, bool checkOnly)
{
    m_configFilename = filename;
    return reload(checkOnly);
}

bool ConfigManager::reload(bool checkOnly)
{
    INFO("Reload config...");

    try
    {
        std::ifstream istrm(m_configFilename);
        nlohmann::json j;
        istrm >> j;

        m_configPtr = std::make_shared<Config>(j);
    }
    catch(const std::ios_base::failure& e)
    {
        CRITICAL("Error open config file: " << e.what());
        return false;
    }
    catch(const nlohmann::json::exception& e)
    {
        CRITICAL("Error parsing config as json: " << e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        CRITICAL("Error processing config: " << e.what());
        return false;
    }
    catch(...)
    {
        CRITICAL("Unknown error!!!");
        return false;
    }

    if(!checkOnly) // If check-config only - do not load logger system
    {
        if(!loadLogger())
        {
            CRITICAL("Error loading logger system...");
            return false;
        }
    }

    if(!m_configPtr->check())
    {
        CRITICAL("Error check config...");
        return false;
    }

    return true;
}

bool ConfigManager::loadLogger()
{
    auto [filename, level, size, count] = m_configPtr->loggerConfig;
    return Logger::getInstance()->init(filename, level, size, count);
}

ConfigPtr ConfigManager::getConfig() {
    return m_configPtr;
}

std::string ConfigManager::getHostName() const noexcept
{
    return m_hostname;
}

std::filesystem::path ConfigManager::getConfigPath() const
{
    return std::filesystem::path(m_configFilename).parent_path();
}