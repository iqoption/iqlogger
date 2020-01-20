//
// Core.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <chrono>

#include <filesystem>

#include <boost/asio.hpp>

#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <nlohmann/json.hpp>

namespace iqlogger::config {

using nlohmann::json;

enum class Protocol
{
  UNDEFINED = 0,
  UDP = 1,
  TCP = 2
};

class ProtocolMap
{
  static constexpr std::pair<Protocol, frozen::string> s_protocol_to_str_map[]{
      {Protocol::UNDEFINED, "undefined"},
      {Protocol::UDP, "udp"},
      {Protocol::TCP, "tcp"},
  };

  static constexpr auto protocol_to_str_map = frozen::make_unordered_map(s_protocol_to_str_map);

  static constexpr std::pair<frozen::string, Protocol> s_protocol_from_str_map[]{
      {"undefined", Protocol::UNDEFINED},
      {"udp", Protocol::UDP},
      {"tcp", Protocol::TCP},
  };

  static constexpr auto protocol_from_str_map = frozen::make_unordered_map(s_protocol_from_str_map);

public:
  static constexpr frozen::string protocol_to_str(Protocol p) { return protocol_to_str_map.at(p); }

  static constexpr Protocol protocol_from_str(frozen::string const& name) { return protocol_from_str_map.at(name); }
};

NLOHMANN_JSON_SERIALIZE_ENUM(Protocol,
                             {
                                 {Protocol::UNDEFINED, ProtocolMap::protocol_to_str(Protocol::UNDEFINED).data()},
                                 {Protocol::UDP, ProtocolMap::protocol_to_str(Protocol::UDP).data()},
                                 {Protocol::TCP, ProtocolMap::protocol_to_str(Protocol::TCP).data()},
                             });
}  // namespace iqlogger::config

namespace nlohmann {

template<>
struct adl_serializer<std::chrono::seconds> {
  static void from_json(const json& j, std::chrono::seconds& interval);
};
}  // namespace nlohmann

std::ostream& operator<<(std::ostream& os, iqlogger::config::Protocol protocol);
