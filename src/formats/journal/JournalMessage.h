//
// JournalMessage.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#define RAPIDJSON_HAS_STDSTRING 1

#include <chrono>
#include <unordered_map>
#include <sstream>

#include <nlohmann/json.hpp>

#include "Exception.h"
#include "formats/MessageInterface.h"
#include "formats/Object.h"

namespace iqlogger::formats::journal {

using JournalRecord = Object;

class JournalMessage : public MessageInterface
{
  JournalRecord m_data;
  uint64_t m_timestamp;

  template<class T>
  struct always_false : std::false_type {};

public:
  using SourceT = void;

  template<typename T>
  explicit JournalMessage(T&& data, uint64_t timestamp) : m_data(std::forward<T>(data)), m_timestamp(timestamp) {}

  template<typename T, typename = std::enable_if_t<std::is_same<std::string, std::decay_t<T>>::value>>
  explicit JournalMessage(T&& json) {
    try {
      nlohmann::json j = nlohmann::json::parse(std::forward<T>(json));
      m_data = j.at("data").get<JournalRecord>();
      m_timestamp = j.at("timestamp").get<uint64_t>();
    } catch (std::exception& e) {
      std::ostringstream oss;
      oss << "Coudn't construct Journal message from " << json << ":" << e.what() << std::endl;
      throw Exception(oss.str());
    }
  }

  JournalMessage(JournalMessage&&) noexcept = default;

  uint64_t getTimestamp() const { return m_timestamp; }

  const JournalRecord& getData() const { return m_data; }

  std::string exportMessage() const override {
    nlohmann::json json(m_data);
    return json.dump();
  }

  std::string exportMessage2Json() const override;

  ~JournalMessage() = default;
};
}
