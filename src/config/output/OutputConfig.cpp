//
// OutputConfig.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "OutputConfig.h"

using namespace iqlogger::config;

void nlohmann::adl_serializer<OutputConfig>::from_json(const json& j, OutputConfig& outputConfig) {
  j.at("threads").get_to(outputConfig.threads);
  j.at("destinations").get_to(outputConfig.destinationsConfigs);
}

void nlohmann::adl_serializer<DestinationConfig>::from_json(const json& j, DestinationConfig& destinationConfig) {
  j.at("name").get_to(destinationConfig.name);
  j.at("type").get_to(destinationConfig.type);
  j.at("threads").get_to(destinationConfig.threads);

  if (auto it = j.find("timeout"); it != j.end()) {
    destinationConfig.timeout = it.value();
  }

  for (auto it = j.begin(); it != j.end(); ++it) {
    const auto& key = it.key();

    if (key != "name" && key != "type" && key != "threads" && key != "timeout") {
      destinationConfig.params.emplace(key, it.value());
    }
  }
}

std::ostream& operator<<(std::ostream& os, OutputType type) {
  return os << DestinationConfig::output_type_to_str(type).data();
}

template<>
std::optional<unsigned short> DestinationConfig::getParam(const std::string& key) const {
  try {
    if (params.at(key).is_number()) {
      return params.at(key).get<unsigned short>();
    }

    if (params.at(key).is_string()) {
      auto v = params.at(key).get<std::string>();
      return std::stoi(std::move(v));
    }
  } catch (const std::exception& e) {
  }

  return std::nullopt;
}