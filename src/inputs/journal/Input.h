//
// Input.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "JournalInput.h"

#define JOURNAL_INPUT_INSTANTIATE(...) \
{\
    return std::make_unique<iqlogger::inputs::journal::JournalInput>(__VA_ARGS__);\
}


