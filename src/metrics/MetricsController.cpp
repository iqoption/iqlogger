//
// MetricsController.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "config/ConfigManager.h"
#include "MetricsController.h"
#include "stats/http/Server.h"
#include "stats/telegraf/Server.h"
#include "Exception.h"

using namespace iqlogger::metrics;

MetricsController::MetricsController() :
    TaskInterface::TaskInterface(),
    m_metricsPtr {std::make_shared<MetricsTableInternal>()},
    m_interval {config::ConfigManager::getInstance()->getConfig()->statsConfig.interval}
{
    for (auto exportConfig : config::ConfigManager::getInstance()->getConfig()->statsConfig.exportConfigs)
    {
        INFO("Creating export for " << exportConfig.name << " " << exportConfig.type);
        m_exportServers.push_back(stats::StatsExportServer::instantiate(exportConfig));
    }
}

void MetricsController::start() {

    INFO("Running metrics thread... ");

    INFO("Add internal HW Metrics...");
    registerInternalMetrics();

    TaskInterface::start();

    m_metrics_thread = std::thread([this]() {

        while(!isStopped())
        {
            m_metricsPtr->processMetrics();
            std::this_thread::sleep_for(std::chrono::seconds(m_interval));
        }
    });

    for(auto& exportServerPtr : m_exportServers)
    {
        m_export_server_threads.emplace_back(std::thread([&exportServerPtr]() {
            exportServerPtr->start();
        }));
    }

    INFO("Started metrics thread...");
}

void MetricsController::stop() {

    TRACE("MetricsController::stop() ->");

    for(auto& exportServerPtr : m_exportServers)
    {
        exportServerPtr->stop();
    }

    TaskInterface::stop();

    TRACE("Join Servers thread... ");

    for(auto& thread : m_export_server_threads)
    {
        if(thread.joinable())
            thread.join();
    }

    TRACE("Join metric thread... ");
    if(m_metrics_thread.joinable())
        m_metrics_thread.join();

    TRACE("MetricsController::stop() <-");
}

bool MetricsController::registerInternalMetrics()
{
    m_internalMetrics = std::make_unique<InternalMetrics>();
    return m_internalMetrics->registerInternalMetrics([this](std::string const& name, metric_callback_t callback) {
        return registerMetric(name, callback);
    });
}

MetricsController::~MetricsController() {
    TRACE("MetricsController::~MetricsController()");
}
