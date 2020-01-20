//
// StatsExportServer.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>

#include "config/Config.h"
#include "core/TaskInterface.h"

namespace iqlogger::stats {

class StatsExportServer;
using StatsExportServerPtr = std::unique_ptr<StatsExportServer>;

class StatsExportServer : public TaskInterface
{
public:
  StatsExportServer() = default;

  StatsExportServer(const StatsExportServer&) = delete;
  StatsExportServer& operator=(const StatsExportServer&) = delete;

  static StatsExportServerPtr instantiate(const config::StatsEntryConfig& statsEntryConfig);

protected:

  void initImpl(std::any) override;

private:
  template<config::StatsEntryType... Types>
  static StatsExportServerPtr instantiateImpl(config::StatsEntryConfig statsEntryConfig) {
    auto dispatcher = utils::types::make_switch(
        [&]() -> StatsExportServerPtr {
          std::stringstream oss;
          oss << "Unknown type " << statsEntryConfig.type;
          throw Exception(oss.str());
        },
        std::make_pair(Types, [&]() { return instantiateStatsExportServer<Types>(statsEntryConfig); })...);
    return dispatcher(statsEntryConfig.type);
  }

  template<config::StatsEntryType Type>
  static StatsExportServerPtr instantiateStatsExportServer(const config::StatsEntryConfig& statsEntryConfig);
};
}  // namespace iqlogger::stats
