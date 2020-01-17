//
// Output.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "LogOutput.h"

// @TODO Params specified

#define LOG_OUTPUT_INSTANTIATE(...) \
{\
    return std::make_unique<iqlogger::outputs::log::LogOutput>(__VA_ARGS__);\
}

