//
// IQLogger.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "IQLogger.h"

using namespace iqlogger;

IQLogger::IQLogger() :
        TaskInterface::TaskInterface(),
        m_signals(m_io_service),
        m_metricsControllerPtr {metrics::MetricsController::getInstance()},
        m_mainMessageQueuePtr {MainMessageQueue::getInstance()},
#ifdef IQLOGGER_WITH_PROCESSOR
        m_processorPtr {processor::Processor::getInstance()},
#endif
        m_inputServerPtr {InputServer::getInstance()},
        m_outputServerPtr {OutputServer::getInstance()}
{
}

void IQLogger::start()
{
    TaskInterface::start();

    INFO("Start IQLogger Server " << config::ConfigManager::getInstance()->getHostName());

#ifdef IQLOGGER_WITH_PROCESSOR
    INFO("Detecting processor support...");
    INFO("Initializing processor...");
    m_processorPtr->start();
#endif

    INFO("Initialize complete... Starting input server...");
    m_inputServerPtr->start();

    INFO("Starting outputs...");
    m_outputServerPtr->start();

    INFO("Initialize complete...");
    INFO("Register signal handlers...");

    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
#if defined(SIGQUIT)
    m_signals.add(SIGQUIT);
#endif // defined(SIGQUIT)

    m_signals.async_wait(
        [this](boost::system::error_code ec, int signo)
        {
            INFO("Received signal " << signo << ". Error: " << ec.message());
            stop();
            shutdown();
        });

    INFO("Start stats server...");
    m_metricsControllerPtr->start();

    INFO("Entering main loop...");

    m_io_service.run();
}

void IQLogger::shutdown()
{
    INFO("Stopping the server...");

    INFO("Shutdown stats server...");
    m_metricsControllerPtr->stop();

    INFO("Shutdown inputs...");
    m_inputServerPtr->stop();

    INFO("Shutdown outputs...");
    m_outputServerPtr->stop();

#ifdef IQLOGGER_WITH_PROCESSOR
    INFO("Shutdown processor...");
    m_processorPtr->stop();
#endif
}
