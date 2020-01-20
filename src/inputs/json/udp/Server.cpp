//
// Server.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Server.h"

using namespace iqlogger::inputs::json::udp;
using namespace iqlogger::formats::json;

void Server::udp_session::process(const boost::system::error_code& error, std::size_t bytes_transferred) {
  auto buffer_begin = m_buffer.cbegin(), buffer_end = m_buffer.cend();
  std::string buff(
      buffer_begin,
      buffer_begin + std::min((std::size_t)std::max(0l, std::distance(buffer_begin, buffer_end)), bytes_transferred));

  TRACE("Receive " << bytes_transferred << " b from " << m_remote_endpoint << ": `" << buff
                   << "` EC: " << error.message());

  try {
    if (!m_queuePtr->enqueue(std::make_unique<Record<Json>>(std::move(buff), Json::SourceT(m_remote_endpoint)))) {
      ERROR("Json UDP Input queue is full... Dropping...");
    }
  } catch (const JsonException& e) {
    WARNING("Buffer decode error: " << e.what() << " from " << m_remote_endpoint);
    DEBUG("Receive " << bytes_transferred << " b: `"
                     << std::string(
                            buffer_begin,
                            buffer_begin + std::min((std::size_t)std::max(0l, std::distance(buffer_begin, buffer_end)),
                                                    bytes_transferred))
                     << "`");
  }
}