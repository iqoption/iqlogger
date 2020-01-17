//
// GelfMessage.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#define RAPIDJSON_HAS_STDSTRING 1

#include <iomanip>
#include <iostream>
#include <sstream>

#include <rapidjson/document.h>
#include <rapidjson/error/error.h>
#include <rapidjson/error/en.h>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "config/ConfigManager.h"
#include "GelfMessage.h"

using namespace iqlogger::formats::gelf;

GelfMessage::gelf::gelf() :
    version     {"1.1"},
    host        {GelfMessage::getHostname()},
    timestamp   {GelfMessage::getCurrentTime()},
    level       {1}
{
}

GelfMessage::gelf::gelf(std::string_view message)
{
    rapidjson::Document document;
    rapidjson::ParseResult ok = document.Parse(message.data());

    if (!ok)
    {
        std::ostringstream oss;
        oss << "Coudn't construct GELF message: " << rapidjson::GetParseError_En(ok.Code()) << std::endl
            << "exception id: " << ok.Code() << std::endl
            << "byte position of error: " << ok.Offset() << std::endl << "Message: " << message << std::endl;
        throw GelfParseError(oss.str());
    }

    if(!document.IsObject())
    {
        throw GelfParseError("GELF buffer is not an object!");
    }

    if(const auto it = document.FindMember("version"); it != document.MemberEnd())
    {
        if(!it->value.IsString())
            throw GelfParseError("Version in GELF must be string");

        version = it->value.GetString();

        if(version != "1.1")
            throw GelfParseError("Version in GELF must be 1.1");
    }
    else
        throw GelfParseError("Version in GELF doesn't present");

    if(auto it = document.FindMember("host"); it != document.MemberEnd())
    {
        if(!it->value.IsString())
            throw GelfParseError("Host in GELF must be string");

        host = it->value.GetString();

        if(host.empty())
            throw GelfParseError("Host in GELF must not be empty");
    }
    else
        throw GelfParseError("Host in GELF doesn't present");

    if(auto it = document.FindMember("short_message"); it != document.MemberEnd())
    {
        if(!it->value.IsString())
            throw GelfParseError("Short message in GELF must be string");

        short_message = it->value.GetString();

        if(short_message.empty())
            throw GelfParseError("Short message in GELF must not be empty");
    }
    else
        throw GelfParseError("hort message in GELF doesn't present");

    if(auto it = document.FindMember("full_message"); it != document.MemberEnd())
    {
        if(!it->value.IsString())
            throw GelfParseError("Full message in GELF must be string");

        full_message = it->value.GetString();
    }

    if(auto it = document.FindMember("timestamp"); it != document.MemberEnd())
    {
        if(it->value.IsLosslessDouble())
        {
            timestamp = it->value.GetDouble();
        }
        else if (it->value.IsString())
        {
            timestamp = std::stod(it->value.GetString());
        }
        else
            throw GelfParseError("Timestamp in GELF must be UNIX time");

        if(timestamp < min_timestamp)
            throw GelfParseError("Incorrect timestamp in GELF. Timestamp in GELF must be UNIX time.");
    }
    else
        timestamp = GelfMessage::getCurrentTime();

    if(auto it = document.FindMember("level"); it != document.MemberEnd())
    {
        if(it->value.IsInt())
        {
            level = it->value.GetInt();
        }
        else if (it->value.IsString())
        {
            level = std::stoi(it->value.GetString());
        }
        else
            throw GelfParseError("Level in GELF must be integer");

        if(level < 1 || level >= 10)
            throw GelfParseError("Incorrect level in GELF");
    }
    else
        level = 1;

    // Non system fields must be with "_"

    for (const auto& m : document.GetObject())
    {
        std::string key {m.name.GetString()};

        if(!isSystemField(key))
        {
            if(key[0] != '_')
                key = '_' + key;

            if(m.value.IsString())
            {
                additionalFields.emplace(key, m.value.GetString());
            }
            else if(m.value.IsInt())
            {
                additionalFields.emplace(key, m.value.GetInt());
            }
            else if(m.value.IsDouble())
            {
                additionalFields.emplace(key, m.value.GetDouble());
            }
            else if(m.value.IsBool())
            {
                additionalFields.emplace(key, m.value.GetBool());
            }
            else
            {
                std::ostringstream oss;
                oss << "Incorrect type in GELF: " << m.name.GetString();
                throw GelfParseError(oss.str());
            }
        }
    }
}

GelfMessage::GelfMessage() : m_gelf()
{
    TRACE("GelfMessage::GelfMessage()");
}

GelfMessage::~GelfMessage()
{
    TRACE("GelfMessage::~GelfMessage()");
}

bool GelfMessage::isSystemField(const std::string& key)
{
    if(
        key == "version" ||
        key == "host" ||
        key == "short_message" ||
        key == "full_message" ||
        key == "timestamp" ||
        key == "level"
        )
    {
        return true;
    }

    return false;
}

double GelfMessage::getCurrentTime()
{
    using clock = std::chrono::system_clock;
    using unixtime = std::chrono::duration<double>;
    return std::chrono::duration_cast<unixtime>(clock::now().time_since_epoch()).count();
}

const std::string& GelfMessage::getHostname()
{
    static const std::string hostname = config::ConfigManager::getInstance()->getHostName();
    return hostname;
}

std::string GelfMessage::gelf::dump() const
{
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();

    writer.Key("version");
    writer.String(version);

    writer.Key("host");
    writer.String(host);

    writer.Key("short_message");
    writer.String(short_message);

    if(!full_message.empty())
    {
        writer.Key("full_message");
        writer.String(full_message);
    }

    writer.Key("timestamp");
    writer.Double(timestamp);

    if(level)
    {
        writer.Key("level");
        writer.Int(level);
    }

    for (const auto& [key, value] : additionalFields)
    {
        writer.Key(key.c_str());

        // @TODO Перенести это в отдельный хелпер
        std::visit([&writer](const auto& v) {

            using T = std::decay_t<decltype(v)>;

            if constexpr (std::is_same_v<T, std::string>)
                writer.String(v);
            else if constexpr (std::is_same_v<T, int>)
                writer.Int(v);
            else if constexpr (std::is_same_v<T, bool>)
                writer.Bool(v);
            else if constexpr (std::is_same_v<T, double>)
                writer.Double(v);
            else
            {
                writer.Null();
                WARNING("Error construct JSON: type must be [int|double|bool|string], but is " << v);
            }

        }, value);
    }

    writer.EndObject();
    return s.GetString();
}