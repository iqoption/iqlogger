//
// InotifyServer.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "InotifyServer.h"
#include "core/Log.h"

using namespace iqlogger;
using namespace iqlogger::inputs::tail;

InotifyServer::InotifyServer() :
    m_inotify_fd{inotify_init1(IN_NONBLOCK)},
    m_io_service{},
    m_stream(m_io_service, m_inotify_fd),
    m_strand(m_io_service),
    m_watchDescriptorsMap() {
  TRACE("InotifyServer::InotifyServer()");
  DEBUG("Create Inotify Descriptor: " << m_inotify_fd);

  if (m_inotify_fd == -1) {
    boost::system::system_error e(boost::system::error_code(errno, boost::asio::error::get_system_category()),
                                  "Init inotify failed");
    boost::throw_exception(e);
  }

  begin_read();
}

void InotifyServer::begin_read() {
  TRACE("InotifyServer::begin_read()");
  auto handler = [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
    end_read(ec, bytes_transferred);
  };

  m_stream.async_read_some(boost::asio::buffer(m_read_buffer), m_strand.wrap(std::move(handler)));
}

void InotifyServer::end_read(const boost::system::error_code& ec, std::size_t bytes_transferred) {
  TRACE("InotifyServer::end_read()");
  TRACE("Inotify EC: " << ec.value() << " (" << ec.message() << ")");

  if (!ec) {
    m_pending_read_buffer += std::string(m_read_buffer.data(), bytes_transferred);
    while (m_pending_read_buffer.size() >= sizeof(inotify_event)) {
      const inotify_event* iev = reinterpret_cast<const inotify_event*>(m_pending_read_buffer.data());

      TRACE("Inotify event WD: " << iev->wd << " Name: " << iev->name);

      switch (iev->mask) {
      case IN_ACCESS:
        TRACE("Inotify IN_ACCESS: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex << iev->mask);
        break;

      case IN_OPEN:
        TRACE("Inotify IN_OPEN: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex << iev->mask);
        break;

      case IN_CLOSE_NOWRITE:
        TRACE("Inotify IN_CLOSE_NOWRITE: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex << iev->mask);
        break;

      case IN_CLOSE_WRITE:
        TRACE("Inotify IN_CLOSE_WRITE: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex << iev->mask);
        // notify(iev->wd, std::make_unique<Event>(iev->wd, event_t::MODIFY, iev->name));
        break;

      case IN_CREATE:
        TRACE("Inotify CREATE: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex << iev->mask);
        notify(iev->wd, std::make_unique<Event>(iev->wd, event_t::CREATE, iev->name));
        break;

      case IN_ATTRIB:
        TRACE("Inotify IN_ATTRIB: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex << iev->mask);
        break;

      case IN_DELETE:
        TRACE("Inotify DELETE: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex << iev->mask);
        notify(iev->wd, std::make_unique<Event>(iev->wd, event_t::DELETE, iev->name));
        break;

      case IN_MOVED_FROM:
        TRACE("Inotify MOVED_FROM: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex << iev->mask);
        notify(iev->wd, std::make_unique<Event>(iev->wd, event_t::MOVE, iev->name));
        break;

      case IN_MOVED_TO:
        TRACE("Inotify MOVED_TO: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex << iev->mask);
        notify(iev->wd, std::make_unique<Event>(iev->wd, event_t::CREATE, iev->name));
        break;

      case IN_CREATE | IN_ISDIR:
        TRACE("Inotify IN_CREATE | IN_ISDIR: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex
                                               << iev->mask);
        break;

      case IN_MOVE_SELF:
        TRACE("Inotify IN_MOVE_SELF: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex << iev->mask);
        break;

      case IN_MODIFY:
        TRACE("Inotify MODIFY: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex << iev->mask);
        notify(iev->wd, std::make_unique<Event>(iev->wd, event_t::MODIFY, iev->name));
        break;

      default:
        DEBUG("Not expected event: " << iev->wd << " Name: " << iev->name << " Mask: " << std::hex << iev->mask);
      }

      m_pending_read_buffer.erase(0, sizeof(inotify_event) + iev->len);
    }

    begin_read();
  } else if (ec != boost::asio::error::operation_aborted) {
    WARNING("Inotify EC: " << ec.value() << " (" << ec.message() << ")");
  }
}

void InotifyServer::notify(fd_t watchDescriptor, EventPtr eventPtr) {
  TRACE("InotifyServer::notify()");

  WatchDescriptorsMap::const_accessor wd_accessor;

  if (m_watchDescriptorsMap.find(wd_accessor, watchDescriptor)) {
    wd_accessor->second->notify(std::move(eventPtr));
  } else {
    ERROR("Error find WATCH WD");
  }
}

void InotifyServer::addWatch(const std::string& path, notifier_t notifier) {
  TRACE("InotifyServer::addWatch()");

  DEBUG("Add WATCH: PATH: `" << path << "`");

  std::string directory;
  std::string pattern;

  if (std::filesystem::is_directory(path)) {
    directory = path;
    pattern = path + "/(.*)";
  } else {
    directory = std::filesystem::path(path).parent_path();
    pattern = path;
  }

  DEBUG("Add WATCH: DIR: `" << directory << "` PATTERN: `" << pattern << "`");

  auto watchDescriptor = inotify_add_watch(m_inotify_fd, directory.c_str(), inotify_events);
  INFO("New listener to directory " << directory << " I_FD: " << m_inotify_fd << " W_FD: " << watchDescriptor);

  if (watchDescriptor == -1) {
    boost::system::system_error e(boost::system::error_code(errno, boost::asio::error::get_system_category()),
                                  "Inotify watch failed");
    boost::throw_exception(e);
  }

  {
    WatchDescriptorsMap::accessor wd_accessor;

    if (m_watchDescriptorsMap.insert(wd_accessor, watchDescriptor)) {
      wd_accessor->second = std::make_unique<Watch>(directory);
    }

    wd_accessor->second->addNotifier(std::move(notifier), std::regex(pattern));
  }

  try {
    for (auto& p : std::filesystem::directory_iterator(directory)) {
      auto filename = p.path().string();
      if (std::regex_match(filename, std::regex(pattern))) {
        DEBUG("Initial read from matched file: " << filename);
        notify(watchDescriptor, std::make_unique<Event>(watchDescriptor, event_t::_INIT, p.path().filename()));
      } else {
        TRACE("NOT Initial read from not matched file: " << filename);
      }
    }
  } catch (const std::filesystem::filesystem_error& e) {
    ERROR("Error read from FS: " << e.what() << " during add watch to " << directory);
  }

  TRACE("InotifyServer::addWatch() <-");
}

void InotifyServer::initImpl(std::any) {
  // @TODO
}

void InotifyServer::startImpl() {
  TRACE("InotifyServer::start()");

  INFO("Start Inotify Server");

  INFO("Start InotifyServer threads...");
  for (size_t i = 0; i < m_thread_num; ++i) {
    m_threads.emplace_back(std::thread([this]() {
      try {
        m_io_service.run();
      } catch (const std::exception& e) {
        CRITICAL("Inotify thread running exited with " << e.what());
      }
    }));
  }
}

void InotifyServer::stopImpl() {
  TRACE("InotifyServer::stop()");
  INFO("Stop Inotify Server");

  m_io_service.stop();

  TRACE("Join threads... ");
  for (auto& t : m_threads) {
    if (t.joinable())
      t.join();
  }
}

InotifyServer::~InotifyServer() {
  TRACE("InotifyServer::~InotifyServer()");
}

InotifyServer::Watch::Watch(std::string directory) : m_directory(std::move(directory)) {
  TRACE("InotifyServer::Watch::Watch()");
}

void InotifyServer::Watch::addNotifier(notifier_t notifier, std::regex regex) {
  TRACE("InotifyServer::Watch::addNotifier()");
  m_notifiers.emplace_back(std::make_unique<InotifyServer::Watch::WatchNotifier>(std::move(regex), std::move(notifier)));
}

void InotifyServer::Watch::notify(EventPtr eventPtr) const {
  TRACE("InotifyServer::Watch::notify()");
  TRACE("Notify: " << eventPtr->m_watchDescriptor << " Name: " << eventPtr->m_filename
                   << " Type: " << eventPtr->m_eventType);

  std::string fullName = m_directory + '/' + eventPtr->m_filename;

  for (auto& notifier : m_notifiers) {
    if (std::regex_match(fullName, notifier->m_regex)) {
      TRACE("Event match: " << fullName);
      notifier->m_notifier(
          std::make_unique<Event>(eventPtr->m_watchDescriptor, eventPtr->m_eventType, std::move(fullName)));
    } else {
      TRACE("Event not match: " << fullName);
    }
  }
}