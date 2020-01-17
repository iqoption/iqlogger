//
// Dummy.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <string>

namespace iqlogger::inputs::dummy {

    class Dummy {

    public:

        using RecordDataT   = std::string;
        using MessageT      = RecordDataT;
        using SourceT       = void;
    };
}



