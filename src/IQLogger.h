//
// IQLogger.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "config/ConfigManager.h"
#include "InputServer.h"
#include "OutputServer.h"
#include "metrics/MetricsController.h"
#include "MainMessageQueue.h"

#ifdef IQLOGGER_WITH_PROCESSOR
#include <processor/Processor.h>
#endif

namespace iqlogger {

    class IQLogger : public Singleton<IQLogger>, public TaskInterface {

        boost::asio::io_service m_io_service;
        boost::asio::signal_set m_signals;

        metrics::MetricsControllerPtr    m_metricsControllerPtr;
        MainMessageQueuePtr     m_mainMessageQueuePtr;
#ifdef IQLOGGER_WITH_PROCESSOR
        processor::ProcessorPtr m_processorPtr;
#endif
        InputServerPtr          m_inputServerPtr;
        OutputServerPtr         m_outputServerPtr;

        explicit IQLogger();
        friend class Singleton<IQLogger>;

    public:

        virtual ~IQLogger() {};

        void start() override;
//        void restart() override;

        void shutdown();
    };
}