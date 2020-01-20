//
// Config.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "config/Config.h"

using namespace iqlogger::config;

Config::Config(const json& j) {
  j.at("iqlogger").at("logger").get_to(loggerConfig);

  j.at("input").get_to(inputConfig);
  j.at("output").get_to(outputConfig);

  j.at("iqlogger").at("stats").get_to(statsConfig);

#ifdef IQLOGGER_WITH_PROCESSOR
  if (auto it = j.at("iqlogger").find("processor"); it != j.at("iqlogger").end()) {
    it->get_to(processorConfig);
  }
#endif
}

bool Config::check() const {
  return (loggerConfig.check() && inputConfig.check() && outputConfig.check() && statsConfig.check()
#ifdef IQLOGGER_WITH_PROCESSOR
          && processorConfig.check()
#endif
  );
}