//
// Exporter.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "nlohmann/json.hpp"

#include "Exporter.h"
#include "metrics/MetricsController.h"

using namespace iqlogger::metrics;
using namespace iqlogger::stats::telegraf;

Exporter::Exporter(const std::string& measurement) : m_measurement(measurement)
{
}

/*
 * Это совершенно мега-костыльная реализация специально для @pavel.boev :-)
 * @FIXME Необходим полный рефакторинг (!!!)
 *
 * */
std::string Exporter::exportTelegraf()
{
    std::ostringstream oss_result;

    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    auto metricsController = MetricsController::getInstance();
    auto metrics = metricsController->getMetrics();

    try
    {
        std::ostringstream oss;

        oss << m_measurement << "_process ";
        oss << "cpu_user_seconds_total="     << metrics.at("process.cpu_user_seconds_total")
            << "i,open_fds="                 << metrics.at("process.open_fds")
            << "i,cpu_system_seconds_total=" << metrics.at("process.cpu_system_seconds_total")
            << "i,resident_memory_bytes="    << metrics.at("process.resident_memory_bytes")
            << "i,max_fds="                  << metrics.at("process.max_fds")
            << "i,threads="                  << metrics.at("process.threads")
            << "i " << time << std::endl;
        oss_result << oss.str();
    }
    catch(const std::out_of_range& ex)
    {
        DEBUG("Exception out of range: " << ex.what());
    }

    boost::property_tree::ptree ptree = getMetricsPtree();

#ifdef IQLOGGER_WITH_PROCESSOR
    try
    {
        std::ostringstream oss;

        oss << m_measurement << "_processor ";
        oss << "queue_length="  << metrics.at("processor.queue.length")
            << "i " << time << std::endl;

        oss_result << oss.str();
    }
    catch(const std::out_of_range& ex)
    {
        DEBUG("Exception out of range: " << ex.what());
    }

    try
    {
        std::ostringstream oss;

        for (boost::property_tree::ptree::value_type &processor : ptree.get_child("processor"))
        {
            if(processor.first != "queue")
            {
                oss << m_measurement << "_processor,processor=" << processor.first
                    << " heap_size="  << metrics.at("processor." + processor.first + ".heap_size")
                    << "i " << time << std::endl;
            }
        }

        oss_result << oss.str();
    }
    catch(const std::out_of_range& ex)
    {
        DEBUG("Exception out of range: " << ex.what());
    }
#endif

    try
    {
        std::ostringstream oss;
        oss << m_measurement << "_main_queue ";
        oss << "queue_length="  << metrics.at("main_queue.length")
            << "i " << time << std::endl;
        oss_result << oss.str();
    }
    catch(const std::out_of_range& ex)
    {
        DEBUG("Exception out of range: " << ex.what());
    }

    try
    {
        std::ostringstream oss;

        for (boost::property_tree::ptree::value_type &input : ptree.get_child("inputs"))
        {
            if(input.first != "receive_total")
            {
                oss  << m_measurement << "_inputs,name=" << input.first << " ";

                short i = 0;

                visit_if(input.second, [&oss, &i](boost::property_tree::ptree::path_type const& path, boost::property_tree::ptree const& node) {

                    if(i++)
                        oss << ",";

                    auto p = path.dump();

                    std::replace(p.begin(), p.end(), '.', '_');

                    oss << p << "=" << node.get_value("") << "i";
                }, is_leaf);

                oss << " " << time << std::endl;
            }
        }

        oss_result << oss.str();
    }
    catch(const std::out_of_range& ex)
    {
        DEBUG("Exception out of range: " << ex.what());
    }

    try
    {
        std::ostringstream oss;

        for (boost::property_tree::ptree::value_type &output : ptree.get_child("outputs"))
        {
            if(output.first != "sent_total")
            {
                oss  << m_measurement << "_outputs,name=" << output.first << " ";

                short i = 0;

                visit_if(output.second, [&oss, &i](boost::property_tree::ptree::path_type const& path, boost::property_tree::ptree const& node) {

                    if(i++)
                        oss << ",";

                    auto p = path.dump();

                    std::replace(p.begin(), p.end(), '.', '_');

                    oss << p << "=" << node.get_value("") << "i";
                }, is_leaf);

                oss << " " << time << std::endl;
            }
        }

        oss_result << oss.str();
    }
    catch(const std::out_of_range& ex)
    {
        DEBUG("Exception out of range: " << ex.what());
    }

    return oss_result.str();
}

boost::property_tree::ptree Exporter::getMetricsPtree()
{
    boost::property_tree::ptree iqlogger;

    auto metricsController = MetricsController::getInstance();

    auto metrics = metricsController->getMetrics();

    for (const auto& m : metrics)
    {
        iqlogger.put(m.first, m.second);
    }

    return iqlogger;
}