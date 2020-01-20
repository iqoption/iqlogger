//
// Input.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Input.h"
#include "inputs/InputInterface.h"
#include "tcp/JsonInput.h"
#include "udp/JsonInput.h"

using namespace iqlogger;
using namespace iqlogger::inputs;

template<>
InputPtr InputInterface::instantiateInput<config::InputType::JSON>(const config::SourceConfig& sourceConfig) {
  if (auto p = sourceConfig.getParam<config::Protocol>("protocol"); p) {
    auto protocol = p.value();
    if (protocol == config::Protocol::UDP) {
      return std::make_unique<iqlogger::inputs::json::udp::JsonInput>(sourceConfig);
    } else if (protocol == config::Protocol::TCP) {
      return std::make_unique<iqlogger::inputs::json::tcp::JsonInput>(sourceConfig);
    } else {
      std::ostringstream oss;
      oss << "Protocol for input " << sourceConfig.name << " is undefined!";
      throw Exception(oss.str());
    }
  } else {
    std::ostringstream oss;
    oss << "Protocol for input " << sourceConfig.name << " not specified!";
    throw Exception(oss.str());
  }
}

template <>
void Module<Input<json::Json>>::initImpl() {
  DEBUG("Module<Input<json::Json>>::initImpl()");
}

template <>
void Module<Input<json::Json>>::startImpl() {
  DEBUG("Module<Input<json::Json>>::startImpl()");
}

template <>
void Module<Input<json::Json>>::stopImpl() {
  DEBUG("Module<Input<json::Json>>::stopImpl()");
}
