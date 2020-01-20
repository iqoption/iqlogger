//
// Event.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Event.h"

using namespace iqlogger;
using namespace iqlogger::inputs::tail;

std::ostream& operator<<(std::ostream& os, event_t event) {
  try {
    return os << Event::event_to_str(event).data();
  } catch (const std::out_of_range& ex) {
    ERROR("Exception: " << ex.what());
    return os;
  }
}