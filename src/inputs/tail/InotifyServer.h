//
// InotifyServer.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <array>
#include <boost/asio.hpp>
#include <iostream>
#include <regex>

#include <sys/inotify.h>

#include <tbb/concurrent_hash_map.h>

#include "core/Singleton.h"
#include "core/Log.h"
#include "core/TaskInterface.h"
#include "Event.h"

namespace iqlogger::inputs::tail {

    class InotifyServer : public Singleton<InotifyServer>, public TaskInterface {

        constexpr static size_t buffer_size = 4096;
        constexpr static size_t inotify_threads_num = 2; // @FIXME Configurable param
        constexpr static uint32_t inotify_events = (IN_ALL_EVENTS | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_MODIFY);

        using buffer_t = std::array<char, buffer_size>;

        friend class Singleton<InotifyServer>;

        class Watch
        {
            struct WatchNotifier {
                const std::regex m_regex;
                const notifier_t m_notifier;

                explicit WatchNotifier(const std::regex& regex, const notifier_t notifier) :
                    m_regex(regex),
                    m_notifier(notifier)
                {}
            };

            using WatchNotifierPtr = std::unique_ptr<WatchNotifier>;

            const std::string m_directory;
            std::vector<WatchNotifierPtr> m_notifiers;

        public:

            explicit Watch(const std::string& directory);
            void addNotifier(const notifier_t notifier, const std::regex& regex);
            void notify(EventPtr eventPtr) const;
        };

        using WatchPtr = std::unique_ptr<Watch>;
        using WatchDescriptorsMap = tbb::concurrent_hash_map<fd_t, WatchPtr>;

        fd_t m_inotify_fd;

        boost::asio::io_service m_io_service;
        boost::asio::posix::stream_descriptor m_stream;
        boost::asio::io_service::strand m_strand;

        size_t m_thread_num = inotify_threads_num;
        std::vector<std::thread> m_threads;

        buffer_t m_read_buffer;
        std::string m_pending_read_buffer;

        WatchDescriptorsMap m_watchDescriptorsMap;

        std::once_flag m_started_flag;
        std::once_flag m_stopped_flag;

        InotifyServer();

        void begin_read();
        void end_read(const boost::system::error_code &ec, std::size_t bytes_transferred);

        void notify(fd_t watchDescriptor, EventPtr eventPtr);

    public:

        virtual ~InotifyServer();

        void addWatch(const std::string& path, notifier_t notifier);

        virtual void start() override;
        virtual void stop() override;
    };
}




