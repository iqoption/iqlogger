//
// Core.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Core.h"

using namespace iqlogger::config;

void nlohmann::adl_serializer<std::chrono::seconds>::from_json(const json& j, std::chrono::seconds& interval) {
  interval = std::chrono::seconds(j.get<long>());
}

std::ostream& operator<<(std::ostream& os, Protocol protocol) {
  return os << ProtocolMap::protocol_to_str(protocol).data();
}