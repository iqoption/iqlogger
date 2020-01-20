//
// Input.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Input.h"
#include "inputs/InputInterface.h"
#include "tcp/GelfInput.h"
#include "udp/GelfInput.h"

using namespace iqlogger;
using namespace iqlogger::inputs;

template<>
InputPtr InputInterface::instantiateInput<config::InputType::GELF>(const config::SourceConfig& sourceConfig) {
  if (auto p = sourceConfig.getParam<config::Protocol>("protocol"); p) {
    auto protocol = p.value();
    if (protocol == config::Protocol::UDP) {
      return std::make_unique<iqlogger::inputs::gelf::udp::GelfInput>(sourceConfig);
    } else if (protocol == config::Protocol::TCP) {
      return std::make_unique<iqlogger::inputs::gelf::tcp::GelfInput>(sourceConfig);
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
void Module<Input<gelf::Gelf>>::initImpl() {
  DEBUG("Module<Input<gelf::Gelf>>::initImpl()");
}

template <>
void Module<Input<gelf::Gelf>>::startImpl() {
  DEBUG("Module<Input<gelf::Gelf>>::startImpl()");
}

template <>
void Module<Input<gelf::Gelf>>::stopImpl() {
  DEBUG("Module<Input<gelf::Gelf>>::stopImpl()");
}