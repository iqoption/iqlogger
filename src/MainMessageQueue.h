//
// MessageQueue.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <MessageQueue.h>

namespace iqlogger {
class MainMessageQueue : public UniqueMessageQueue, public Singleton<MainMessageQueue>
{
  friend class Singleton<MainMessageQueue>;
  explicit MainMessageQueue() : UniqueMessageQueue("main_queue") {}
};

using MainMessageQueuePtr = std::shared_ptr<MainMessageQueue>;
}