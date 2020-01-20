//
// StatsExportServer.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "stats/StatsExportServer.h"
#include "Server.h"

using namespace iqlogger;
using namespace iqlogger::stats;

template<>
StatsExportServerPtr StatsExportServer::instantiateStatsExportServer<config::StatsEntryType::TELEGRAF>(
    const config::StatsEntryConfig& statsEntryConfig) {
  auto measurement = statsEntryConfig.getParam<std::string>("measurement");
  if (!measurement) {
    std::ostringstream oss;
    oss << "Measurement for export " << statsEntryConfig.name << " not specified!";
    throw Exception(oss.str());
  }

  auto host = statsEntryConfig.getParam<std::string>("host");
  if (!host) {
    std::ostringstream oss;
    oss << "Host for export " << statsEntryConfig.name << " not specified!";
    throw Exception(oss.str());
  }

  auto port = statsEntryConfig.getParam<unsigned short>("port");
  if (!port) {
    std::ostringstream oss;
    oss << "Port for export " << statsEntryConfig.name << " not specified!";
    throw Exception(oss.str());
  }

  auto interval = statsEntryConfig.getParam<unsigned short>("interval");
  if (!interval) {
    std::ostringstream oss;
    oss << "Interval for export " << statsEntryConfig.name << " not specified!";
    throw Exception(oss.str());
  }

  return std::make_unique<iqlogger::stats::telegraf::Server>(measurement.value(), host.value(), port.value(),
                                                             interval.value());
}
