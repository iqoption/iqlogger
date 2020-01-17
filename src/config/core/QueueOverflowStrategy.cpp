//
// QueueOverflowStrategy.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "QueueOverflowStrategy.h"

using namespace iqlogger::config;

std::ostream& operator<< (std::ostream& os, QueueOverflowStrategy strategy)
{
    return os << QueueOverflowStrategyMap::strategy_to_str(strategy).data();
}