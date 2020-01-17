//
// ConfigException.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "Exception.h"

namespace iqlogger::config {

    class ConfigException : public iqlogger::Exception {
        using iqlogger::Exception::Exception;
    };

}

