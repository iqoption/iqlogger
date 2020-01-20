//
// QueueOverflowStrategy.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "QueueOverflowStrategy.h"

using namespace iqlogger::config;

std::ostream& operator<<(std::ostream& os, const QueueOverflowStrategy::Type& strategy) {
  return os << QueueOverflowStrategy::strategy_to_str(strategy).data();
}