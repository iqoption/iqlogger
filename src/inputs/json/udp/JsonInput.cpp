//
// JsonInput.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "JsonInput.h"
#include <metrics/MetricsController.h>

using namespace iqlogger::inputs::json::udp;
using namespace iqlogger::formats::json;

JsonInput::JsonInput(const config::SourceConfig& sourceConfig) : IOInput::IOInput(sourceConfig) {
  if (auto port = sourceConfig.getParam<unsigned short>("port"); port) {
    m_port = port.value();
  } else {
    std::ostringstream oss;
    oss << "Port for input " << m_name << " not specified!";
    throw Exception(oss.str());
  }

  INFO("Register metrics");

  m_serverPtr = std::make_shared<Server>(m_inputQueuePtr, m_io_service, m_port);
}

void JsonInput::startImpl() {
  TRACE("udp::JsonInput::start()");
  IOInput::startImpl();
}

void JsonInput::stopImpl() {
  TRACE("JsonInput::stop ->");
  IOInput::stopImpl();
  TRACE("JsonInput::stop <-");
}

JsonInput::~JsonInput() {
  TRACE("udp::JsonInput::~JsonInput()");
}