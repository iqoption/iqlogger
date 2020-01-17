//
// Input.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "TailInput.h"

#define TAIL_INPUT_INSTANTIATE(...) \
{\
    return std::make_unique<iqlogger::inputs::tail::TailInput>(__VA_ARGS__);\
}


