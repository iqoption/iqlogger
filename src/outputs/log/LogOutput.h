//
// LogOutput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "outputs/Output.h"
#include "outputs/log/Log.h"

namespace iqlogger::outputs::log {

class LogOutput : public Output<Log>
{
  std::function<void()> m_outputThreadCallback;  // @TODO Remove Later

public:
  explicit LogOutput(config::DestinationConfig destinationConfig, metrics::atomic_metric_t& total_outputs_send_counter);

protected:

  void startImpl() override;
  void stopImpl() override;
};

}  // namespace iqlogger::outputs::log
