//
// SavePositionServer.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <filesystem>

#include <tbb/concurrent_hash_map.h>

#include "Tail.h"
#include "core/Log.h"
#include "core/Singleton.h"
#include "core/TaskInterface.h"

namespace iqlogger::inputs::tail {

class SavePositionServer : public Singleton<SavePositionServer>, public TaskInterface
{
  friend class Singleton<SavePositionServer>;
  using SavedPointersTableInternal = tbb::concurrent_hash_map<std::string, Position>;

public:
  virtual ~SavePositionServer();

  Position getSavedPosition(const std::string& filename);
  void savePosition(const std::string& filename, Position position);
  void erasePosition(const std::string& filename);

protected:
  void initImpl(std::any) override;
  void startImpl() override;
  void stopImpl() override;

private:

  SavePositionServer();

  Position getCurrentSavedPositionFromFile(const std::string& filename);

  std::string getSavedPathByFileName(std::string_view filename);

private:

  SavedPointersTableInternal m_savedPointersTableInternal;

  constexpr static frozen::string data_dir = "/var/lib/iqlogger";
};
}  // namespace iqlogger::inputs::tail
