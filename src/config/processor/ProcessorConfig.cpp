//
// ProcessorConfig.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "ProcessorConfig.h"

using namespace iqlogger::config;

void nlohmann::adl_serializer<ProcessorConfig>::from_json(const json& j, ProcessorConfig& processorConfig)
{
    j.at("threads").get_to(processorConfig.threads);
}

