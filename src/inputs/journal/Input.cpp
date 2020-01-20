//
// Input.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Input.h"
#include "JournalInput.h"
#include "inputs/InputInterface.h"

using namespace iqlogger;
using namespace iqlogger::inputs;

template<>
InputPtr InputInterface::instantiateInput<config::InputType::JOURNAL>(const config::SourceConfig& sourceConfig) {
  return std::make_unique<iqlogger::inputs::journal::JournalInput>(sourceConfig);
}

template <>
void Module<Input<journal::Journal>>::initImpl() {
  DEBUG("Module<Input<journal::Journal>>::initImpl()");
}

template <>
void Module<Input<journal::Journal>>::startImpl() {
  DEBUG("Module<Input<journal::Journal>>::startImpl()");
}

template <>
void Module<Input<journal::Journal>>::stopImpl() {
  DEBUG("Module<Input<journal::Journal>>::stopImpl()");
}
