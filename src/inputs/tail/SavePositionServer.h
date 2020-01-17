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

#include "core/Singleton.h"
#include "core/Log.h"
#include "core/TaskInterface.h"
#include "Tail.h"

namespace iqlogger::inputs::tail {

    class SavePositionServer : public Singleton<SavePositionServer>, public TaskInterface {

        constexpr static frozen::string data_dir = "/var/lib/iqlogger";

        using SavedPointersTableInternal = tbb::concurrent_hash_map<std::string, Position>;

        friend class Singleton<SavePositionServer>;

        SavePositionServer();

        std::once_flag m_started_flag;
        std::once_flag m_stopped_flag;

        SavedPointersTableInternal m_savedPointersTableInternal;

        void init();
        Position getCurrentSavedPositionFromFile(const std::string& filename);

        std::string getSavedPathByFileName(std::string_view filename);

    public:

        virtual ~SavePositionServer();

        virtual void start() override;
        virtual void stop() override;

        Position getSavedPosition(const std::string& filename);
        void savePosition(const std::string& filename, Position position);
        void erasePosition(const std::string& filename);
    };
}




