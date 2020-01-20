//
// Connection.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "MimeTypes.h"

using namespace iqlogger::stats::http;

MimeType::mapping_type MimeType::mappings[] = {{"gif", "image/gif"},  {"htm", "text/html"},
                                               {"html", "text/html"}, {"jpg", "image/jpeg"},
                                               {"png", "image/png"},  {"json", "application/json"}};

std::string MimeType::extension_to_type(const std::string& extension) {
  for (mapping m : mappings) {
    if (m.extension == extension) {
      return m.mime_type;
    }
  }

  return "text/plain";
}