//
// Server.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <boost/asio.hpp>
#include <chrono>
#include <string>

#include "Exporter.h"
#include "stats/StatsExportServer.h"

namespace iqlogger::stats::telegraf {

class Server : public StatsExportServer
{
public:
  Server(const std::string& measurement, const std::string& host, unsigned short port, unsigned short interval);

protected:

  void startImpl() override;
  void stopImpl() override;

private:
  boost::asio::io_service m_io_service;
  std::string m_host;
  unsigned short m_port;
  std::chrono::seconds m_interval;
  Exporter m_exporter;
};
}  // namespace iqlogger::stats::telegraf
