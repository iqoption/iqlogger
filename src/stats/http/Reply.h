//
// Reply.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <boost/asio.hpp>
#include "Header.h"
#include <string>
#include <vector>

namespace iqlogger::stats::http {

struct Reply {
  enum class status_type
  {
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
  } status;

  std::vector<Header> headers;

  std::string content;

  std::vector<boost::asio::const_buffer> to_buffers();

  static Reply stock_reply(status_type status);
};
}  // namespace iqlogger::stats::http
