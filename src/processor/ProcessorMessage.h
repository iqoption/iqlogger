//
// ProcessorMessage.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "formats/MessageInterface.h"
#include "core/Log.h"

namespace iqlogger::processor {

class ProcessorMessage : public formats::MessageInterface
{
public:
  ~ProcessorMessage() = default;

  template<typename T, typename = std::enable_if_t<std::is_same<std::string, std::decay_t<T>>::value>>
  explicit ProcessorMessage(T&& json) : m_data(std::forward<T>(json)) {
    TRACE("ProcessorMessage() " << json);
  }

  std::string exportMessage() const override {
    return m_data;
  }

  // @TODO (???)
  std::string exportMessage2Json() const override {
    return m_data;
  }

  ProcessorMessage(ProcessorMessage&&) noexcept = default;

private:
  std::string m_data;
};
}  // namespace iqlogger::processor
