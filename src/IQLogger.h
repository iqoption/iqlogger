//
// IQLogger.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

#include "InputServer.h"
#include "MainMessageQueue.h"
#include "OutputServer.h"
#include "config/ConfigManager.h"
#include "metrics/MetricsController.h"

namespace iqlogger {

class IQLogger : public Singleton<IQLogger>, public TaskInterface
{
  friend class Singleton<IQLogger>;

public:
  ~IQLogger() = default;

protected:
  void initImpl(std::any) override;
  void startImpl() override;
  void stopImpl() override;

private:
  explicit IQLogger();

  boost::asio::io_context m_io_context;
  boost::asio::signal_set m_signals;

  metrics::MetricsControllerPtr m_metricsControllerPtr;
  MainMessageQueuePtr m_mainMessageQueuePtr;
  InputServerPtr m_inputServerPtr;
  OutputServerPtr m_outputServerPtr;
};
}