//
// Tail.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "formats/tail/TailMessage.h"

namespace iqlogger::inputs::tail {

    using Position = int64_t;

    class Tail {

    public:

        using RecordDataT   = std::string;
        using MessageT      = iqlogger::formats::tail::TailMessage;
        using SourceT       = MessageT::SourceT;
    };
}



