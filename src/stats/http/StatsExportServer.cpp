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
StatsExportServerPtr StatsExportServer::instantiateStatsExportServer<config::StatsEntryType::HTTP>(
    const config::StatsEntryConfig& statsEntryConfig) {
  auto listen_host = statsEntryConfig.getParam<std::string>("listen_host");
  if (!listen_host) {
    std::ostringstream oss;
    oss << "Host for export " << statsEntryConfig.name << " not specified!";
    throw Exception(oss.str());
  }

  auto listen_port = statsEntryConfig.getParam<unsigned short>("listen_port");
  if (!listen_port) {
    std::ostringstream oss;
    oss << "Port for export " << statsEntryConfig.name << " not specified!";
    throw Exception(oss.str());
  }

  return std::make_unique<iqlogger::stats::http::Server>(listen_host.value(), listen_port.value());
}
