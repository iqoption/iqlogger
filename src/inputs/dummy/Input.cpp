//
// Input.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Input.h"
#include "Module.h"
#include "DummyInput.h"
#include "inputs/InputInterface.h"

using namespace iqlogger;
using namespace iqlogger::inputs;

template<>
InputPtr InputInterface::instantiateInput<config::InputType::DUMMY>(const config::SourceConfig& sourceConfig) {
  return std::make_unique<iqlogger::inputs::dummy::DummyInput>(sourceConfig);
}

template <>
void Module<Input<dummy::Dummy>>::initImpl() {
  DEBUG("Module<Input<dummy::Dummy>>::initImpl()");
}

template <>
void Module<Input<dummy::Dummy>>::startImpl() {
  DEBUG("Module<Input<dummy::Dummy>>::startImpl()");
}

template <>
void Module<Input<dummy::Dummy>>::stopImpl() {
  DEBUG("Module<Input<dummy::Dummy>>::stopImpl()");
}


