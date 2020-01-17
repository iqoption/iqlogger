//
// Gelf.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <string>
#include <variant>

#include "formats/gelf/GelfMessage.h"

namespace iqlogger::inputs::gelf {

    class Gelf {

    public:

        using RecordDataT   = std::string;
        using MessageT      = iqlogger::formats::gelf::GelfMessage;
        using SourceT       = MessageT::SourceT;
    };
}
