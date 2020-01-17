//
// Object.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <variant>
#include <nlohmann/json.hpp>

namespace iqlogger::formats {

    using ObjectVariants = std::variant<std::string, int, double, bool>;
    using Object = std::unordered_map<std::string, ObjectVariants>;
}

namespace nlohmann {

    template <>
    struct adl_serializer<iqlogger::formats::ObjectVariants>
    {
        static void to_json(json& j, const iqlogger::formats::ObjectVariants& value)
        {
            std::visit([&j](const auto& v) {
                j = v;
            }, value);
        }

        static void from_json(const json& j, iqlogger::formats::ObjectVariants& value)
        {
            if(j.is_number_integer()) {
                value = j.get<int>();
            }
            else if(j.is_number_float()) {
                value = j.get<double>();
            }
            else if(j.is_boolean()) {
                value = j.get<bool>();
            }
            else if(j.is_string()) {
                value = j.get<std::string>();
            } else
                throw detail::type_error::create(302, "type must be [int|double|bool|string], but is " + std::string(j.type_name()));
        }
    };
}

