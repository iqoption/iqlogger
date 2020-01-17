//
// Request.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <string>
#include <vector>

#include "Header.h"

namespace iqlogger::stats::http
{
    struct Request
    {
      std::string method;
      std::string uri;
      int http_version_major;
      int http_version_minor;
      std::vector<Header> headers;
    };
}


