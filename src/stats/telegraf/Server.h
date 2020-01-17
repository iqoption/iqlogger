//
// Server.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <chrono>
#include <boost/asio.hpp>
#include <string>

#include "stats/StatsExportServer.h"
#include "Exporter.h"

namespace iqlogger::stats::telegraf {

    class Server : public StatsExportServer
    {
        boost::asio::io_service m_io_service;

        std::string m_host;
        unsigned short m_port;

        std::chrono::seconds    m_interval;

        Exporter                m_exporter;

    public:

        Server(const std::string& measurement, const std::string& host, unsigned short port, unsigned short interval);

        virtual void start() override;
        virtual void stop() override;
    };
}

#define TELEGRAF_STATS_INSTANTIATE(...)\
{\
    auto measurement = statsEntryConfig.getParam<std::string>("measurement");\
    if(!measurement)\
    {\
        std::ostringstream oss;\
        oss << "Measurement for export " << statsEntryConfig.name << " not specified!";\
        throw Exception(oss.str());\
    }\
    auto host = statsEntryConfig.getParam<std::string>("host");\
    if(!host)\
    {\
        std::ostringstream oss;\
        oss << "Host for export " << statsEntryConfig.name << " not specified!";\
        throw Exception(oss.str());\
    }\
    auto port = statsEntryConfig.getParam<unsigned short>("port");\
    if(!port)\
    {\
        std::ostringstream oss;\
        oss << "Port for export " << statsEntryConfig.name << " not specified!";\
        throw Exception(oss.str());\
    }\
    auto interval = statsEntryConfig.getParam<unsigned short>("interval");\
    if(!port)\
    {\
        std::ostringstream oss;\
        oss << "Interval for export " << statsEntryConfig.name << " not specified!";\
        throw Exception(oss.str());\
    }\
    return std::make_unique<iqlogger::stats::telegraf::Server>(measurement.value(), host.value(), port.value(), interval.value());\
}


