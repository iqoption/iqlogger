//
// RequestHandler.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include <chrono>
#include <fstream>
#include <sstream>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

#include "Version.h"
#include "RequestHandler.h"
#include "MimeTypes.h"
#include "Reply.h"
#include "Request.h"

#include "metrics/MetricsController.h"

using namespace iqlogger::stats::http;
using namespace iqlogger::metrics;

RequestHandler::RequestHandler()
{
}

void RequestHandler::handle_request(const Request& req, Reply& rep)
{
    std::string request_path;

    if (!url_decode(req.uri, request_path))
    {
        rep = Reply::stock_reply(Reply::status_type::bad_request);
        WARNING("HTTP Stats: bad request: " << req.uri);
        return;
    }

    if (request_path.empty() || request_path[0] != '/'
        || request_path.find("..") != std::string::npos)
    {
        rep = Reply::stock_reply(Reply::status_type::bad_request);
        WARNING("HTTP Stats: bad request: " << req.uri);
        return;
    }

    if(request_path == "/stats")
    {
        rep.status = Reply::status_type::ok;

        auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

        boost::property_tree::ptree root;
        boost::property_tree::ptree iqlogger;

        root.put("time", time);
        iqlogger.put("version", IQLOGGER_VERSION);

        auto metricsController = MetricsController::getInstance();

        auto metrics = metricsController->getMetrics();

        for (const auto& m : metrics)
        {
            iqlogger.put(m.first, m.second);
        }

        root.add_child("iqlogger", iqlogger);

        std::stringstream ss;
        boost::property_tree::json_parser::write_json(ss, root);

        rep.content = ss.str();

        rep.headers.resize(2);
        rep.headers[0].name = "Content-Length";
        rep.headers[0].value = std::to_string(rep.content.size());
        rep.headers[1].name = "Content-Type";
        rep.headers[1].value = MimeType::extension_to_type("json");
    }
    else
    {
        rep = Reply::stock_reply(Reply::status_type::not_found);
        WARNING("HTTP Stats: not found: " << req.uri);
        return;
    }
}

bool RequestHandler::url_decode(const std::string& in, std::string& out) {

    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i)
    {
      if (in[i] == '%')
      {
        if (i + 3 <= in.size())
        {
          int value = 0;
          std::istringstream is(in.substr(i + 1, 2));
          if (is >> std::hex >> value)
          {
            out += static_cast<char>(value);
            i += 2;
          }
          else
          {
            return false;
          }
        }
        else
        {
          return false;
        }
      }
      else if (in[i] == '+')
      {
        out += ' ';
      }
      else
      {
        out += in[i];
      }
    }
    return true;
}

