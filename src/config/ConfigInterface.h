//
// Config.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "config/core/Core.h"

namespace iqlogger::config {

struct ConfigInterface {
  ConfigInterface() noexcept = default;
  ConfigInterface(ConfigInterface const&) = default;

  virtual ~ConfigInterface() = default;

  virtual bool check() const { return true; }
};
}  // namespace iqlogger::config
