//
// JsonException.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <iostream>

#include "Exception.h"

namespace iqlogger::formats::json {

    class JsonException : public Exception {
        using Exception::Exception;
    };

    class JsonParseError : public JsonException {
        using JsonException::JsonException;
    };
}


