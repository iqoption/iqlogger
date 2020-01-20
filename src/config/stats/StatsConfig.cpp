//
// StatsConfig.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "StatsConfig.h"

using namespace iqlogger::config;

void nlohmann::adl_serializer<StatsConfig>::from_json(const json& j, StatsConfig& statsConfig) {
  j.at("interval").get_to(statsConfig.interval);
  j.at("export").get_to(statsConfig.exportConfigs);
}

void nlohmann::adl_serializer<StatsEntryConfig>::from_json(const json& j, StatsEntryConfig& statsEntryConfig) {
  j.at("name").get_to(statsEntryConfig.name);
  j.at("type").get_to(statsEntryConfig.type);

  for (auto it = j.begin(); it != j.end(); ++it) {
    const auto& key = it.key();

    if (key != "name" && key != "type") {
      statsEntryConfig.params.emplace(key, it.value());
    }
  }
}

std::ostream& operator<<(std::ostream& os, StatsEntryType type) {
  return os << StatsEntryConfig::stats_type_to_str(type).data();
}