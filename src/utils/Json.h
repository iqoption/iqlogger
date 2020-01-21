//
// Json.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#define RAPIDJSON_HAS_STDSTRING 1

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/error/error.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace iqlogger::utils::json {

template<typename V>
void writeValue(rapidjson::Writer<rapidjson::StringBuffer>& writer, V&& value) {
  std::visit(
      [&](auto&& v) {
        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<T, std::string>)
          writer.String(v);
        else if constexpr (std::is_same_v<T, int>)
          writer.Int(v);
        else if constexpr (std::is_same_v<T, bool>)
          writer.Bool(v);
        else if constexpr (std::is_same_v<T, double>)
          writer.Double(v);
        else {
          writer.Null();
          WARNING("Error construct JSON: type must be [int|double|bool|string], but is " << v);
        }
      },
      std::forward<V>(value));
}

}  // namespace iqlogger::utils::json
