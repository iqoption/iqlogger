//
// GelfChunk.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "GelfChunk.h"
#include "formats/gelf/GelfException.h"

using namespace iqlogger::inputs::gelf::udp;
using namespace iqlogger::formats::gelf;

GelfChunk::GelfChunk(const std::string& chunk, boost::asio::ip::udp::endpoint source) : m_source(std::move(source)) {
  if (chunk.size() <= 12)
    throw GelfException("Chunk length less than 12 bytes!");

  auto it = chunk.begin();

  if (*it == '\x1e') {
    it++;
    if (*it == '\x0f')
      it++;
  }

  auto it2 = it + 8;

  m_messageId = std::string(it, it2);
  m_sequenceNumber = (char)*it2++;
  m_sequenceCount = (char)*it2++;
  m_chunk = std::string(it2, chunk.end());
}

std::string GelfChunk::getMessageId() const {
  return m_messageId;
}

size_t GelfChunk::getSequenceNumber() const {
  return m_sequenceNumber;
}

size_t GelfChunk::getSequenceCount() const {
  return m_sequenceCount;
}

std::string GelfChunk::getChunk() const {
  return m_chunk;
}

boost::asio::ip::udp::endpoint GelfChunk::getSource() const {
  return m_source;
}
