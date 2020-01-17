//
// QueueOverflowStrategy.h
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <nlohmann/json.hpp>

#include <frozen/unordered_map.h>
#include <frozen/string.h>

namespace iqlogger::config {

    using nlohmann::json;

    enum class QueueOverflowStrategy {
        UNDEFINED   = 0,
        THROTTLE    = 1,
        DROP        = 2
    };

    class QueueOverflowStrategyMap
    {
        static constexpr std::pair<QueueOverflowStrategy, frozen::string> s_strategy_to_str_map[]
        {
            {QueueOverflowStrategy::UNDEFINED,  "undefined"},
            {QueueOverflowStrategy::THROTTLE,   "throttle"},
            {QueueOverflowStrategy::DROP,       "drop"},
        };

        static constexpr auto strategy_to_str_map = frozen::make_unordered_map(s_strategy_to_str_map);

        static constexpr std::pair<frozen::string, QueueOverflowStrategy> s_strategy_from_str_map[]
        {
            {"undefined",   QueueOverflowStrategy::UNDEFINED},
            {"throttle",    QueueOverflowStrategy::THROTTLE},
            {"drop",        QueueOverflowStrategy::DROP},
        };

        static constexpr auto strategy_from_str_map = frozen::make_unordered_map(s_strategy_from_str_map);

    public:

        static constexpr frozen::string strategy_to_str(QueueOverflowStrategy p)
        {
            return strategy_to_str_map.at(p);
        }

        static constexpr QueueOverflowStrategy strategy_from_str(frozen::string const& name)
        {
            return strategy_from_str_map.at(name);
        }
    };

    inline void from_json(const json& j, QueueOverflowStrategy& strategy)
    {
        try
        {
            const auto str = j.get<std::string>();
            strategy = QueueOverflowStrategyMap::strategy_from_str(frozen::string(str.data(), str.size()));
        }
        catch(const json::exception& e)
        {
            strategy = QueueOverflowStrategy::UNDEFINED;
        }
    }
}

std::ostream& operator<< (std::ostream& os, iqlogger::config::QueueOverflowStrategy strategy);

