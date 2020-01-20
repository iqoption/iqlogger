//
// StatsExportServer.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "StatsExportServer.h"

using namespace iqlogger::stats;

StatsExportServerPtr StatsExportServer::instantiate(const config::StatsEntryConfig& statsEntryConfig) {
  return instantiateImpl<config::StatsEntryType::HTTP, config::StatsEntryType::TELEGRAF>(statsEntryConfig);
}

void StatsExportServer::initImpl(std::any) {
  // @TODO
}
