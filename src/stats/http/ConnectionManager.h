//
// Connection.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <set>

#include "Connection.h"

namespace iqlogger::stats::http {

    class ConnectionManager
    {
    public:
        ConnectionManager(const ConnectionManager&) = delete;
        ConnectionManager& operator=(const ConnectionManager&) = delete;

        ConnectionManager();

        void start(ConnectionPtr c);

        void stop(ConnectionPtr c);

        void stop_all();

    private:

      std::set<ConnectionPtr> m_connections;

    };
}

