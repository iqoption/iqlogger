//
// OutputServer.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "OutputServer.h"
#include "MessageQueue.h"
#include "metrics/MetricsController.h"
#include "config/ConfigManager.h"
#include "MainMessageQueue.h"

using namespace iqlogger;
using namespace iqlogger::outputs;

OutputServer::OutputServer() :
    TaskInterface::TaskInterface(),
    m_thread_num {config::ConfigManager::getInstance()->getConfig()->outputConfig.threads},
    m_send_counter{0}
{
    INFO("Configure output server...");

    for (auto dstConfig : config::ConfigManager::getInstance()->getConfig()->outputConfig.destinationsConfigs)
    {
        INFO("Configure output destination " << dstConfig.type << " " << dstConfig.name);
        m_outputs.push_back(OutputInterface::instantiate(dstConfig, m_send_counter));
    }

    INFO("Configure output main loop " << m_thread_num << " threads");
}

void OutputServer::start()
{
    // 1. Start outputs
    INFO("Start output server...");

    TaskInterface::start();

    for (const auto& outputPtr : m_outputs)
    {
        outputPtr->start();
    }

    // Запускаем потоки, которые будут перекладывать сообщения из главной очереди в локальные очереди аутпутов

    for (size_t i = 0; i < m_thread_num; ++i)
    {
        m_threads.emplace_back(std::thread([this]() {

            auto messageQueuePtr = MainMessageQueue::getInstance();

            auto& message_buffer = getMessagePtrBuffer();

            std::size_t count;

            while(!isStopped())
            {
                count = messageQueuePtr->try_dequeue_bulk(message_buffer.begin());

                if(count)
                {
                    TRACE("Export " << count << " messages from main queue");

                    for(const auto& outputPtr : m_outputs)
                    {
                        outputPtr->importMessages(message_buffer.cbegin(), count);
                    }
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }

        }));
    }

    INFO ("Register metrics");

    metrics::MetricsController::getInstance()->registerMetric("outputs.sent_total", [this]() {
        return m_send_counter.load();
    });

    INFO("Output server is ready for send logs...");
}

void OutputServer::stop() {

    TRACE("OutputServer::stop() ->");

    for (const auto& outputPtr : m_outputs)
    {
        outputPtr->stop();
    }

    TaskInterface::stop();

    TRACE("Join outputs io threads... ");
    for(auto& t : m_threads)
    {
        if(t.joinable())
            t.join();
    }

    TRACE("OutputServer::stop() <-");
}

OutputServer::~OutputServer() {
    TRACE("OutputServer::~OutputServer()");
}

