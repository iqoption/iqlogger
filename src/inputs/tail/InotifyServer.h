//
// InotifyServer.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <boost/asio.hpp>
#include <array>
#include <iostream>
#include <regex>

#include <sys/inotify.h>

#include <tbb/concurrent_hash_map.h>

#include "Event.h"
#include "core/Log.h"
#include "core/Singleton.h"
#include "core/TaskInterface.h"

namespace iqlogger::inputs::tail {

class InotifyServer : public Singleton<InotifyServer>, public TaskInterface
{
  friend class Singleton<InotifyServer>;

  class Watch
  {
    struct WatchNotifier {
      const std::regex m_regex;
      const notifier_t m_notifier;

      explicit WatchNotifier(std::regex regex, notifier_t notifier) :
          m_regex(std::move(regex)),
          m_notifier(std::move(notifier)) {}
    };

    using WatchNotifierPtr = std::unique_ptr<WatchNotifier>;

    const std::string m_directory;
    std::vector<WatchNotifierPtr> m_notifiers;

  public:
    explicit Watch(std::string directory);
    void addNotifier(notifier_t notifier, std::regex regex);
    void notify(EventPtr eventPtr) const;
  };

  constexpr static size_t buffer_size = 4096;
  constexpr static size_t inotify_threads_num = 2;
  constexpr static uint32_t inotify_events = (IN_ALL_EVENTS | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_MODIFY);

  using WatchPtr = std::unique_ptr<Watch>;
  using WatchDescriptorsMap = tbb::concurrent_hash_map<fd_t, WatchPtr>;
  using buffer_t = std::array<char, buffer_size>;

public:
  virtual ~InotifyServer();
  void addWatch(const std::string& path, notifier_t notifier);

protected:
  void initImpl(std::any) override;
  void startImpl() override;
  void stopImpl() override;

private:
  InotifyServer();

  void begin_read();
  void end_read(const boost::system::error_code& ec, std::size_t bytes_transferred);

  void notify(fd_t watchDescriptor, EventPtr eventPtr);

  fd_t m_inotify_fd;

  boost::asio::io_service m_io_service;
  boost::asio::posix::stream_descriptor m_stream;
  boost::asio::io_service::strand m_strand;

  size_t m_thread_num = inotify_threads_num;
  std::vector<std::thread> m_threads;

  buffer_t m_read_buffer;
  std::string m_pending_read_buffer;

  WatchDescriptorsMap m_watchDescriptorsMap;
};
}  // namespace iqlogger::inputs::tail
