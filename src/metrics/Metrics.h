//
// Metrics.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <atomic>
#include <string>
#include <unordered_map>

namespace iqlogger::metrics {

using metric_t = std::uint_fast64_t;
using atomic_metric_t = std::atomic<metric_t>;
using metric_callback_t = std::function<metric_t()>;
using register_metric_callback_t = std::function<bool(std::string const& name, metric_callback_t callback)>;

using MetricsTable = std::unordered_map<std::string, metric_t>;
using MetricHandlersTable = std::unordered_map<std::string, metric_callback_t>;
}  // namespace iqlogger::metrics
