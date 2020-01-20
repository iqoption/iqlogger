//
// LoggerConfig.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "LoggerConfig.h"

using namespace iqlogger::config;

void nlohmann::adl_serializer<LoggerConfig>::from_json(const json& j, LoggerConfig& loggerConfig) {
  j.at("filename").get_to(loggerConfig.filename);
  j.at("level").get_to(loggerConfig.level);

  if (auto it = j.find("size"); it != j.end()) {
    it->get_to(loggerConfig.size);
  }

  if (auto it = j.find("count"); it != j.end()) {
    it->get_to(loggerConfig.count);
  }
}
