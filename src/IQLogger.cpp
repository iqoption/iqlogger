//
// IQLogger.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "IQLogger.h"

#ifdef IQLOGGER_WITH_PROCESSOR
#include "processor/ProcessorMachine.h"
#endif

using namespace iqlogger;

IQLogger::IQLogger() :
    m_signals(m_io_context),
    m_metricsControllerPtr{metrics::MetricsController::getInstance()},
    m_mainMessageQueuePtr{MainMessageQueue::getInstance()},
    m_inputServerPtr{InputServer::getInstance()},
    m_outputServerPtr{OutputServer::getInstance()} {
}

void IQLogger::initImpl(std::any) {
  // @TODO
}

void IQLogger::startImpl() {
  INFO("Start IQLogger Server " << config::ConfigManager::getInstance()->getHostName());

#ifdef IQLOGGER_WITH_PROCESSOR
  INFO("Detecting processor support...");
  INFO("Initializing processor...");
  processor::ProcessorMachine::getInstance()->init({});
  INFO("Starting processor...");
  processor::ProcessorMachine::getInstance()->start();
#endif

  INFO("Initialize complete... Starting input server...");
  m_inputServerPtr->init({});
  m_inputServerPtr->start();

  INFO("Starting outputs...");
  m_outputServerPtr->init({});
  m_outputServerPtr->start();

  INFO("Initialize complete...");
  INFO("Register signal handlers...");

  m_signals.add(SIGINT);
  m_signals.add(SIGTERM);
#if defined(SIGQUIT)
  m_signals.add(SIGQUIT);
#endif  // defined(SIGQUIT)

  m_signals.async_wait([this](boost::system::error_code ec, int signo) {
    INFO("Received signal " << signo << ". Error: " << ec.message());
    stop();
  });

  INFO("Start stats server...");
  m_metricsControllerPtr->init({});
  m_metricsControllerPtr->start();

  INFO("Entering main loop...");

  m_io_context.run();
}

void IQLogger::stopImpl() {
  INFO("Stopping the server...");

  INFO("Shutdown stats server...");
  m_metricsControllerPtr->stop();

  INFO("Shutdown inputs...");
  m_inputServerPtr->stop();

  INFO("Shutdown outputs...");
  m_outputServerPtr->stop();

#ifdef IQLOGGER_WITH_PROCESSOR
  INFO("Shutdown processor...");
  processor::ProcessorMachine::getInstance()->stop();
#endif
}
