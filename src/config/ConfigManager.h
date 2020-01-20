//
// ConfigManager.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <filesystem>

#include "config/Config.h"
#include "core/Singleton.h"

#ifndef HOST_NAME_MAX
#if defined(_POSIX_HOST_NAME_MAX)
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#elif defined(MAXHOSTNAMELEN)
#define HOST_NAME_MAX MAXHOSTNAMELEN
#endif
#endif

namespace iqlogger::config {

class ConfigManager : public Singleton<ConfigManager>
{
  friend class Singleton<ConfigManager>;

  std::filesystem::path m_configFilename;
  std::string m_hostname;

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
}  // namespace iqlogger::config
