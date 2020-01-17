//
// Input.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "DummyInput.h"

#define DUMMY_INPUT_INSTANTIATE(...) \
{\
    return std::make_unique<iqlogger::inputs::dummy::DummyInput>(__VA_ARGS__);\
}


