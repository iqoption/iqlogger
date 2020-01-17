//
// JournalMessage.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#define RAPIDJSON_HAS_STDSTRING 1

#include <unordered_map>
#include <chrono>

#include <nlohmann/json.hpp>

#include "formats/MessageInterface.h"
#include "formats/Object.h"
#include "Exception.h"

namespace iqlogger::formats::journal {

    using JournalRecord = Object;

    class JournalMessage : public MessageInterface {

        JournalRecord m_data;
        uint64_t m_timestamp;

        template<class T> struct always_false : std::false_type {};

    public:

        using SourceT = void;

        template <typename T>
        explicit JournalMessage(T&& data, uint64_t timestamp) : m_data(std::forward<T>(data)), m_timestamp(timestamp)
        {
        }

        template <typename T, typename = std::enable_if_t<std::is_same<std::string, std::decay_t<T>>::value>>
        explicit JournalMessage(T&& json)
        {
            try
            {
                nlohmann::json j = nlohmann::json::parse(std::forward<T>(json));
                m_data      = j.at("data").get<JournalRecord>();
                m_timestamp = j.at("timestamp").get<uint64_t>();
            }
            catch (std::exception& e)
            {
                std::ostringstream oss;
                oss << "Coudn't construct Journal message from " << json << ":" << e.what() << std::endl;
                throw Exception(oss.str());
            }
        }

        JournalMessage(JournalMessage&&) noexcept = default;

        uint64_t getTimestamp() const
        {
            return m_timestamp;
        }

        const JournalRecord& getData() const
        {
            return m_data;
        }

        std::string exportMessage() const override
        {
            nlohmann::json json(m_data);
            return json.dump();
        }

        std::string exportMessage2Json() const override
        {
            rapidjson::StringBuffer s;
            rapidjson::Writer<rapidjson::StringBuffer> writer(s);

            writer.StartObject();
            writer.Key("data");

            writer.StartObject();

            for (const auto& [key, value] : m_data)
            {
                writer.Key(key.c_str());

                // @TODO Перенести это в отдельный хелпер
                std::visit([&writer](const auto& v) {

                    using T = std::decay_t<decltype(v)>;

                    if constexpr (std::is_same_v<T, std::string>)
                        writer.String(v);
                    else if constexpr (std::is_same_v<T, int>)
                        writer.Int(v);
                    else if constexpr (std::is_same_v<T, double>)
                        writer.Double(v);
                    else if constexpr (std::is_same_v<T, bool>)
                        writer.Bool(v);
                    else
                    {
                        writer.Null();
                        WARNING("Error construct JSON: type must be [int|double|bool|string], but is " << v);
                    }

                }, value);
            }

            writer.EndObject();

            writer.Key("timestamp");
            writer.Int64(m_timestamp);
            writer.EndObject();
            return s.GetString();
        }

        ~JournalMessage() = default;
    };
}


