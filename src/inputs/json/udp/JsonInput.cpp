//
// JsonInput.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include <metrics/MetricsController.h>
#include "JsonInput.h"

using namespace iqlogger::inputs::json::udp;
using namespace iqlogger::formats::json;

JsonInput::JsonInput(const config::SourceConfig& sourceConfig) :
    IOInput::IOInput(sourceConfig)
{
    if(auto port = sourceConfig.getParam<unsigned short>("port"); port)
    {
        m_port = port.value();
    }
    else
    {
        std::ostringstream oss;
        oss << "Port for input " << m_name << " not specified!";
        throw Exception(oss.str());
    }

    INFO ("Register metrics");

    m_serverPtr = std::make_shared<Server>(m_inputQueuePtr, m_io_service, m_port);
}

void JsonInput::start()
{
    TRACE("udp::JsonInput::start()");
    IOInput::start();
}

void JsonInput::stop() {

    TRACE("JsonInput::stop ->");
    IOInput::stop();
    TRACE("JsonInput::stop <-");
}

JsonInput::~JsonInput() {
    TRACE("udp::JsonInput::~JsonInput()");
}