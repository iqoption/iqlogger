//
// Json.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <string>
#include <variant>

#include <boost/asio.hpp>

#include "formats/json/JsonMessage.h"

namespace iqlogger::inputs::json {

    class Json {

    public:

        using RecordDataT   = std::string;
        using MessageT      = iqlogger::formats::json::JsonMessage;
        using SourceT       = MessageT::SourceT;
    };
}



