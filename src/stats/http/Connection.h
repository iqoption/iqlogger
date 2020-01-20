//
// Connection.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <boost/asio.hpp>
#include <array>
#include <memory>

#include "Reply.h"
#include "Request.h"
#include "RequestHandler.h"
#include "RequestParser.h"

namespace iqlogger::stats::http {

class ConnectionManager;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

  explicit Connection(boost::asio::io_service& io_service, ConnectionManager& manager, RequestHandler& handler);

  void start();

  void stop();

  boost::asio::ip::tcp::socket& socket() { return m_socket; }

private:
  void do_read();

  void do_write();

  boost::asio::ip::tcp::socket m_socket;

  ConnectionManager& m_connectionManager;

  RequestHandler& m_requestHandler;

  std::array<char, 8192> m_buffer;

  Request m_request;

  RequestParser m_request_parser;

  Reply m_reply;
};

using ConnectionPtr = std::shared_ptr<Connection>;
}  // namespace iqlogger::stats::http
