//
// Event.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>

#include "config/Config.h"
#include "core/Log.h"

namespace iqlogger::inputs::tail {

using fd_t = int;

struct Event {
  enum class EventType
  {
    UNKNOWN,
    _INIT,
    CREATE,
    MODIFY,
    MOVE,
    DELETE
  };

  fd_t m_watchDescriptor;
  EventType m_eventType;
  std::string m_filename;

  template<typename T>
  explicit Event(fd_t wd, EventType event, T&& filename) :
      m_watchDescriptor(wd),
      m_eventType(event),
      m_filename(std::forward<std::string>(filename)) {
    TRACE("Event::Event()");
  }

  ~Event() { TRACE("Event::~Event()"); }

private:
  static constexpr std::pair<EventType, frozen::string> s_event_to_name_map[]{
      {EventType::UNKNOWN, "UNDEFINED"}, {EventType::_INIT, "_INIT"}, {EventType::CREATE, "CREATE"},
      {EventType::MODIFY, "MODIFY"},     {EventType::MOVE, "MOVE"},   {EventType::DELETE, "DELETE"},
  };

  static constexpr auto event_to_str_map = frozen::make_unordered_map(s_event_to_name_map);

public:
  static constexpr frozen::string event_to_str(EventType type) { return event_to_str_map.at(type); }
};

using event_t = Event::EventType;
using EventPtr = std::unique_ptr<Event>;

using notifier_t = std::function<void(EventPtr)>;

}  // namespace iqlogger::inputs::tail

std::ostream& operator<<(std::ostream& os, const iqlogger::inputs::tail::event_t event);
