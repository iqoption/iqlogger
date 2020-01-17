//
// ConnectionManager.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "ConnectionManager.h"

using namespace iqlogger::stats::http;

ConnectionManager::ConnectionManager()
{
}

void ConnectionManager::start(ConnectionPtr c)
{
    m_connections.insert(c);
    c->start();
}

void ConnectionManager::stop(ConnectionPtr c)
{
    m_connections.erase(c);
    c->stop();
}

void ConnectionManager::stop_all()
{
    for (auto c: m_connections)
        c->stop();
    m_connections.clear();
}
