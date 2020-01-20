//
// ProcessorConfig.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "config/ConfigInterface.h"

namespace iqlogger::config {

struct ProcessorConfig : public ConfigInterface {
  size_t threads = 1;

  ProcessorConfig() noexcept = default;
};
}  // namespace iqlogger::config

namespace nlohmann {

template<>
struct adl_serializer<iqlogger::config::ProcessorConfig> {
  static void from_json(const json& j, iqlogger::config::ProcessorConfig& processorConfig);
};
}  // namespace nlohmann
