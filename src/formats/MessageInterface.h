//
// MessageInterface.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace iqlogger::formats {

class MessageInterface
{
public:
  MessageInterface() noexcept = default;
  MessageInterface(MessageInterface&&) noexcept = default;

  MessageInterface(const MessageInterface&) = delete;
  MessageInterface& operator=(const MessageInterface&) = delete;

  virtual std::string exportMessage() const = 0;
  virtual std::string exportMessage2Json() const = 0;

  virtual ~MessageInterface() = default;
};
}  // namespace iqlogger::formats
