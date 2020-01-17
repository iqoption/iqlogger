//
// TailMessage.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "formats/MessageInterface.h"
#include "Exception.h"

namespace iqlogger::formats::tail {

    class TailMessage : public MessageInterface {

        std::string m_data;
        std::string m_filename;

    public:

        using SourceT = std::string;

        template<typename T, typename U>
        explicit TailMessage(T&& data, U&& filename) : m_data(std::forward<T>(data)), m_filename(std::forward<U>(filename)) {};

        template <
            typename T,
            typename = std::enable_if_t<std::is_same<std::string, std::decay_t<T>>::value>
        >
        explicit TailMessage(T&& json)
        {
            try
            {
                nlohmann::json j = nlohmann::json::parse(std::forward<T>(json));

                if(j.at("data").is_object())
                {
                    m_data = j.at("data").dump();
                }
                else
                {
                    m_data = j.at("data").get<std::string>();
                }

                m_filename = j.at("filename").get<std::string>();
            }
            catch (std::exception& e)
            {
                std::ostringstream oss;
                oss << "Coudn't construct Tail message from " << json << ":" << e.what() << std::endl;
                throw Exception(oss.str());
            }
        }

        TailMessage(TailMessage&&) noexcept = default;

        std::string exportMessage() const override
        {
            return m_data;
        }

        std::string getFilename() const
        {
            return m_filename;
        }

        std::string getSource() const
        {
            return m_filename;
        }

        std::string exportMessage2Json() const override
        {
            rapidjson::StringBuffer s;
            rapidjson::Writer<rapidjson::StringBuffer> writer(s);

            writer.StartObject();
            writer.Key("data");
            writer.String(m_data);
            writer.Key("filename");
            writer.String(m_filename);
            writer.EndObject();
            return s.GetString();
        }

        ~TailMessage() = default;
    };
}


