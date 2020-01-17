//
// Server.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include <utility>

#include "Server.h"
#include "core/Log.h"

using namespace iqlogger::stats::http;

Server::Server(const std::string& address, unsigned short port):
    StatsExportServer(),
    m_io_service(),
    m_acceptor(m_io_service),
    m_connectionManager(),
    m_requestHandler()
{
    INFO("HTTP Metrics Server starting on " << address << ":" << port);

    boost::asio::ip::tcp::resolver resolver(m_io_service);
    boost::asio::ip::tcp::resolver::query q{address, std::to_string(port)};
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(q);
    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen();

    do_accept();
}

void Server::start()
{
    m_io_service.run();
}

void Server::do_accept()
{
    auto connectionPtr = std::make_shared<Connection>(m_io_service, m_connectionManager, m_requestHandler);

    m_acceptor.async_accept(connectionPtr->socket(),
      [this, connectionPtr](boost::system::error_code ec)
      {
          if (!m_acceptor.is_open())
          {
            return;
          }

          if (!ec)
          {
            m_connectionManager.start(connectionPtr);
          }

          do_accept();
    });
}

void Server::stop()
{
    m_io_service.stop();
    m_acceptor.close();
    m_connectionManager.stop_all();
}

