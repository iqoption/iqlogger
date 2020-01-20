//
// RequestHandler.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <string>

namespace iqlogger::stats::http {

struct Reply;
struct Request;

class RequestHandler
{
public:
  RequestHandler(const RequestHandler&) = delete;
  RequestHandler& operator=(const RequestHandler&) = delete;

  explicit RequestHandler();

  void handle_request(const Request& req, Reply& rep);

private:
  static bool url_decode(const std::string& in, std::string& out);
};
}  // namespace iqlogger::stats::http
