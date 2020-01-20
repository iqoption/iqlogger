//
// TailInput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <filesystem>

#include "Tail.h"
#include "TailMonitor.h"
#include "inputs/Input.h"

namespace iqlogger::inputs::tail {

class TailInput : public Input<Tail>
{
  enum class DelimiterType
  {
    NEWLINE = 0,
    REGEX = 1
  };

public:
  explicit TailInput(const config::SourceConfig& sourceConfig);
  virtual ~TailInput();

protected:

  void startImpl() override;
  void stopImpl() override;

private:
  std::string m_path;

  DelimiterType m_delimiterType;

  unsigned short m_read_timeout;

  TailMonitorPtr m_monitorPtr;

  std::thread m_timerThread;

  constexpr static unsigned short default_read_timeout = 10;
  constexpr static bool default_follow_only_mode = false;
};
}  // namespace iqlogger::inputs::tail
