//
// GelfInput.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "GelfInput.h"
#include <metrics/MetricsController.h>

using namespace iqlogger::inputs::gelf::udp;
using namespace iqlogger::formats::gelf;

GelfInput::GelfInput(const config::SourceConfig& sourceConfig) :
    IOInput::IOInput(sourceConfig),
    m_chunkQueuePtr{std::make_shared<ChunkQueue>()},
    m_bad_chunks{0} {
  m_chunk_ttl = sourceConfig.getParam<unsigned short>("chunked_timeout").value_or(5);

  if (auto port = sourceConfig.getParam<unsigned short>("port"); port) {
    m_port = port.value();
  } else {
    std::ostringstream oss;
    oss << "Port for input " << m_name << " not specified!";
    throw Exception(oss.str());
  }

  INFO("Register metrics");

  metrics::MetricsController::getInstance()->registerMetric("inputs." + m_name + ".bad_chunks",
                                                            [this]() { return m_bad_chunks.load(); });

  m_serverPtr = std::make_shared<Server>(m_inputQueuePtr, m_chunkQueuePtr, m_io_service, m_port);
}

void GelfInput::startImpl() {
  TRACE("udp::GelfInput::start()");

  IOInput::startImpl();

  m_chunkProcessThread = std::thread([this]() {
    constexpr size_t chunk_bulk_limit = 1000;

    typedef std::map<std::size_t, GelfChunkPtr> GelfChunks;

    typedef struct {
      GelfChunks chunks;
      std::size_t size;
      std::chrono::steady_clock::time_point time;

    } ChunkedMessage;

    typedef std::shared_ptr<ChunkedMessage> ChunkedMessagePtr;

    std::unordered_map<std::string, ChunkedMessagePtr> chunkMap;

    GelfChunkPtr bulk[chunk_bulk_limit];
    std::size_t count;

    while (isRunning()) {
      count = m_chunkQueuePtr->try_dequeue_bulk(bulk, chunk_bulk_limit);

      if (count) {
        for (std::size_t j = 0; j < count; ++j) {
          auto chunkPtr = bulk[j];

          auto msgId = chunkPtr->getMessageId();
          auto chunksCount = chunkPtr->getSequenceCount();
          auto chunkNumber = chunkPtr->getSequenceNumber();

          if (chunksCount > 128) {
            WARNING("Chunks count " << chunksCount << " out of range...");
            continue;
          }

          if (chunkNumber >= chunksCount) {
            WARNING("Chunk number " << chunkNumber << " out of range " << chunksCount << "...");
            continue;
          }

          auto messageIt = chunkMap.find(msgId);

          if (messageIt != chunkMap.end()) {
            auto messagePtr = messageIt->second;

            if (messagePtr->size != chunksCount) {
              WARNING("Something strange in GELF...");
              // @TODO Drop message at all
              continue;
            }

            messagePtr->chunks[chunkNumber] = chunkPtr;

            if (messagePtr->chunks.size() == messagePtr->size) {
              std::stringstream totalMessage;

              for (const auto& chunk : messagePtr->chunks) {
                totalMessage << chunk.second->getChunk();
              }

              TRACE("GELF Total message: `" << totalMessage.str() << "`");

              try {
                if (auto it = messagePtr->chunks.begin(); it != messagePtr->chunks.end()) {
                  if (!m_inputQueuePtr->enqueue(
                          std::make_unique<Record<Gelf>>(totalMessage.str(), it->second->getSource()))) {
                    ERROR("Gelf UDP Input queue is full... Dropping...");
                  }
                } else {
                  WARNING("Chunks without endpoint!");
                }

              } catch (const GelfException& e) {
                WARNING("Chunk Buffer decode error... Reason: " << e.what());
              }

              chunkMap.erase(messageIt);
            }
          } else {
            auto messagePtr = std::make_shared<ChunkedMessage>();

            messagePtr->size = chunksCount;
            messagePtr->time = std::chrono::steady_clock::now();
            messagePtr->chunks[chunkNumber] = chunkPtr;
            chunkMap.insert(std::make_pair(msgId, messagePtr));
          }
        }
      }

      // Проверяем прокисшие чанки
      auto now = std::chrono::steady_clock::now();

      for (auto it = chunkMap.cbegin(); it != chunkMap.cend();) {
        if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second->time).count() >= m_chunk_ttl) {
          auto exampleChunk = it->second->chunks.cbegin();
          WARNING("Chunk Buffer erase... TTL... " << it->second->chunks.size() << "/" << it->second->size << " from "
                                                  << exampleChunk->second->getSource());
          it = chunkMap.erase(it);
          ++m_bad_chunks;
        } else {
          ++it;
        }
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });
}

void GelfInput::stopImpl() {
  TRACE("GelfInput::stop ->");

  IOInput::stopImpl();

  if (m_chunkProcessThread.joinable())
    m_chunkProcessThread.join();

  TRACE("GelfInput::stop <-");
}

GelfInput::~GelfInput() {
  TRACE("udp::GelfInput::~GelfInput()");
}