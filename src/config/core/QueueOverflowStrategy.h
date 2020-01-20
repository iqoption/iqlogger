//
// QueueOverflowStrategy.h
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <chrono>
#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <nlohmann/json.hpp>

namespace iqlogger::config {

using nlohmann::json;

class QueueOverflowStrategy
{
public:
  enum class Type
  {
    UNDEFINED = 0,
    THROTTLE = 1,
    DROP = 2
  };

  static inline constexpr auto throttle_interval = std::chrono::milliseconds(100);

private:
  static constexpr std::pair<Type, frozen::string> s_strategy_to_str_map[]{
      {Type::UNDEFINED, "undefined"},
      {Type::THROTTLE, "throttle"},
      {Type::DROP, "drop"},
  };

  static constexpr auto strategy_to_str_map = frozen::make_unordered_map(s_strategy_to_str_map);

  static constexpr std::pair<frozen::string, Type> s_strategy_from_str_map[]{
      {"undefined", Type::UNDEFINED},
      {"throttle", Type::THROTTLE},
      {"drop", Type::DROP},
  };

  static constexpr auto strategy_from_str_map = frozen::make_unordered_map(s_strategy_from_str_map);

public:
  static constexpr frozen::string strategy_to_str(Type p) { return strategy_to_str_map.at(p); }

  static constexpr Type strategy_from_str(frozen::string const& name) { return strategy_from_str_map.at(name); }
};

inline void from_json(const json& j, QueueOverflowStrategy::Type& strategy) {
  try {
    const auto str = j.get<std::string>();
    strategy = QueueOverflowStrategy::strategy_from_str(frozen::string(str.data(), str.size()));
  } catch (const json::exception& e) {
    strategy = QueueOverflowStrategy::Type::UNDEFINED;
  }
}
}  // namespace iqlogger::config

std::ostream& operator<<(std::ostream& os, const iqlogger::config::QueueOverflowStrategy::Type& strategy);
