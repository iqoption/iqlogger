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

        virtual void start() override;
        virtual void stop() override;

    private:

        void do_accept();

        boost::asio::io_service m_io_service;
        boost::asio::ip::tcp::acceptor m_acceptor;
        ConnectionManager m_connectionManager;
        RequestHandler m_requestHandler;
    };
}

#define HTTP_STATS_INSTANTIATE(...)\
{\
    auto listen_host = statsEntryConfig.getParam<std::string>("listen_host");\
    if(!listen_host)\
    {\
        std::ostringstream oss;\
        oss << "Host for export " << statsEntryConfig.name << " not specified!";\
        throw Exception(oss.str());\
    }\
    auto listen_port = statsEntryConfig.getParam<unsigned short>("listen_port");\
    if(!listen_port)\
    {\
        std::ostringstream oss;\
        oss << "Port for export " << statsEntryConfig.name << " not specified!";\
        throw Exception(oss.str());\
    }\
    return std::make_unique<iqlogger::stats::http::Server>(listen_host.value(), listen_port.value());\
}


