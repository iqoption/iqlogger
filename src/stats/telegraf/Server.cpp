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

using namespace iqlogger::stats::telegraf;

Server::Server(const std::string& measurement, const std::string& host, unsigned short port, unsigned short interval) :
    m_io_service(),
    m_host(host),
    m_port(port),
    m_interval(std::chrono::seconds(interval)),
    m_exporter(measurement) {
  INFO("Telegraf Exporter Metrics Server starting to " << host << ":" << port << " Interval: " << interval << " s.");
}

void Server::startImpl() {

  boost::asio::ip::udp::socket socket(m_io_service);
  boost::asio::ip::udp::resolver resolver(m_io_service);
  boost::asio::ip::udp::resolver::query q{m_host, std::to_string(m_port)};
  boost::asio::ip::udp::endpoint endpoint = *resolver.resolve(q);

  socket.open(boost::asio::ip::udp::v4());

  while (isRunning()) {
    if (auto metrics = m_exporter.exportTelegraf(); !metrics.empty()) {
      try {
        socket.send_to(boost::asio::buffer(metrics), endpoint);
        m_io_service.run_one();
      } catch (const boost::system::system_error& error) {
        TRACE("Send `" << metrics << "`");
        WARNING("Error send metrics to " << endpoint << ": " << error.what());
      }
    }

    std::this_thread::sleep_for(m_interval);
  }
}

void Server::stopImpl() {
  m_io_service.stop();
}
