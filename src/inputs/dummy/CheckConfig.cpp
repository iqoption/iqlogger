//
// CheckConfig.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "config/Config.h"

using namespace iqlogger;
using namespace iqlogger::config;

template<>
bool SourceConfigCheckImpl<InputType::DUMMY>::operator()([[maybe_unused]] const SourceConfig& sourceConfig) const {
  return true;
}