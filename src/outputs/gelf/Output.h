//
// Input.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "tcp/GelfOutput.h"

// @TODO Params specified

#define GELF_OUTPUT_INSTANTIATE(...) \
{\
    if(destinationConfig.getParam<config::Protocol>("protocol") == config::Protocol::TCP) {\
        return std::make_unique<outputs::gelf::tcp::GelfOutput>(__VA_ARGS__);\
    }\
}

