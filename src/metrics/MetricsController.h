//
// MetricsController.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <mutex>
#include <shared_mutex>

#include <core/Singleton.h>
#include <core/TaskInterface.h>

#include "config/Config.h"
#include "Metrics.h"
#include "InternalMetrics.h"
#include "stats/StatsExportServer.h"

namespace iqlogger::metrics {

    class MetricsController : public Singleton<MetricsController>, public TaskInterface {

        friend class Singleton<MetricsController>;

        class MetricsTableInternal
        {
            mutable std::shared_timed_mutex m_mutex;
            MetricHandlersTable m_metricsCallbacks;
            MetricsTable m_currentMetrics;

        public:

            MetricsTableInternal() {};

            bool registerMetric(std::string const& name, metric_callback_t callback)
            {
                std::unique_lock<std::shared_timed_mutex> lock(m_mutex);
                m_currentMetrics.insert(std::make_pair(name, 0));
                m_metricsCallbacks.insert(std::make_pair(name, callback));
                return true;
            }

            bool processMetrics()
            {
                std::unique_lock<std::shared_timed_mutex> lock(m_mutex);
                for (const auto& m : m_metricsCallbacks)
                {
                    m_currentMetrics[m.first] = m.second();
                }

                return true;
            }

            MetricsTable getMetrics() const {

                std::shared_lock<std::shared_timed_mutex> lock(m_mutex);
                return m_currentMetrics;
            };
        };

        using MetricsTableInternalPtr = std::shared_ptr<MetricsTableInternal>;

        MetricsTableInternalPtr m_metricsPtr;

        std::thread m_metrics_thread;

        std::vector<std::thread> m_export_server_threads;
        std::vector<stats::StatsExportServerPtr> m_exportServers;

        size_t m_interval;

        std::unique_ptr<InternalMetrics> m_internalMetrics;

        MetricsController();

    public:

        virtual ~MetricsController();

        virtual void start() override;
        virtual void stop() override;

        bool registerMetric(std::string const& name, metric_callback_t callback)
        {
            return m_metricsPtr->registerMetric(name, callback);
        }

        MetricsTable getMetrics() const {

            return m_metricsPtr->getMetrics();
        };

        bool registerInternalMetrics();
    };

    using MetricsControllerPtr = std::shared_ptr<MetricsController>;
}

