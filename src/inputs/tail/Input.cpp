//
// Input.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Input.h"
#include "TailInput.h"
#include "inputs/InputInterface.h"
#include "SavePositionServer.h"

using namespace iqlogger;
using namespace iqlogger::inputs;
using namespace iqlogger::inputs::tail;

template<>
InputPtr InputInterface::instantiateInput<config::InputType::TAIL>(const config::SourceConfig& sourceConfig) {
  return std::make_unique<iqlogger::inputs::tail::TailInput>(sourceConfig);
}

template <>
void Module<Input<tail::Tail>>::initImpl() {
  DEBUG("Module<Input<tail::Tail>>::initImpl()");
}

template <>
void Module<Input<tail::Tail>>::startImpl() {
  DEBUG("Module<Input<tail::Tail>>::startImpl()");

  INFO("Start Save Position Server");
  SavePositionServer::getInstance()->init({});
  SavePositionServer::getInstance()->start();

  INFO("Start Inotify Server");
  InotifyServer::getInstance()->init({});
  InotifyServer::getInstance()->start();
}

template <>
void Module<Input<tail::Tail>>::stopImpl() {
  DEBUG("Module<Input<tail::Tail>>::stopImpl()");

  INFO("Stop Save Position Server");
  SavePositionServer::getInstance()->stop();

  INFO("Stop Inotify Server");
  InotifyServer::getInstance()->stop();
}
