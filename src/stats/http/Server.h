//
// Server.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <boost/asio.hpp>
#include <string>

#include "Connection.h"
#include "ConnectionManager.h"
#include "RequestHandler.h"
#include "stats/StatsExportServer.h"

namespace iqlogger::stats::http {

class Server : public StatsExportServer
{
public:
  explicit Server(const std::string& address, unsigned short port);

protected:

  void startImpl() override;
  void stopImpl() override;

private:

  void do_accept();

  boost::asio::io_service m_io_service;
  boost::asio::ip::tcp::acceptor m_acceptor;
  ConnectionManager m_connectionManager;
  RequestHandler m_requestHandler;
};
}  // namespace iqlogger::stats::http
