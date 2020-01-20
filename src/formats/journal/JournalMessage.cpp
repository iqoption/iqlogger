//
// JournalMessage.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#define RAPIDJSON_HAS_STDSTRING 1

#include "JournalMessage.h"
#include "utils/Json.h"

using namespace iqlogger::formats::journal;

std::string JournalMessage::exportMessage2Json() const {
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  writer.StartObject();
  writer.Key("data");

  writer.StartObject();

  for (const auto& [key, value] : m_data) {
    writer.Key(key.c_str());
    utils::json::writeValue(writer, value);
  }

  writer.EndObject();

  writer.Key("timestamp");
  writer.Int64(m_timestamp);
  writer.EndObject();
  return s.GetString();
}