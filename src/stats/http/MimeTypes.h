//
// Connection.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <string>

namespace iqlogger::stats::http {

    struct MimeType {

        using mapping_type = struct mapping
        {
            const char* extension;
            const char* mime_type;
        };

        static mapping_type mappings[];
        static std::string extension_to_type(const std::string& extension);

    };
}

