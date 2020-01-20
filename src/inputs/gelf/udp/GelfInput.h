//
// GelfInput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "Server.h"
#include <inputs/IOInput.h>

namespace iqlogger::inputs::gelf::udp {

class GelfInput : public IOInput<Gelf>
{
public:
  explicit GelfInput(const config::SourceConfig& sourceConfig);

  virtual ~GelfInput();

protected:
  void startImpl() override;
  void stopImpl() override;

private:
  std::shared_ptr<Server> m_serverPtr;
  ChunkQueuePtr m_chunkQueuePtr;
  std::thread m_chunkProcessThread;
  std::atomic<uint64_t> m_bad_chunks;
  unsigned short m_chunk_ttl;
  unsigned short m_port;
};
}  // namespace iqlogger::inputs::gelf::udp
