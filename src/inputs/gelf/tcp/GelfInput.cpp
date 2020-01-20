//
// GelfInput.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "GelfInput.h"

using namespace iqlogger::inputs::gelf::tcp;

GelfInput::GelfInput(const config::SourceConfig& sourceConfig) : IOInput::IOInput(sourceConfig) {
  if (auto port = sourceConfig.getParam<unsigned short>("port"); port) {
    m_port = port.value();
  } else {
    std::ostringstream oss;
    oss << "Port for input " << m_name << " not specified!";
    throw Exception(oss.str());
  }

  m_serverPtr = std::make_shared<Server>(m_inputQueuePtr, m_io_service, m_port);
}

GelfInput::~GelfInput() {
  TRACE("tcp::GelfInput::~GelfInput()");
}

void GelfInput::startImpl() {
  TRACE("tcp::GelfInput::start()");
  IOInput::startImpl();
}
