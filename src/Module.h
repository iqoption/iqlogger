//
// Module.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <mutex>

#include <core/Singleton.h>
#include <core/TaskInterface.h>

namespace iqlogger {

template<typename T>
class Module : public Singleton<Module<T>>
{
  friend class Singleton<Module<T>>;

public:

  ~Module() = default;

  void init() {
    std::call_once(m_inited_flag, [this](){ initImpl(); });
  }

  void start() {
    std::call_once(m_started_flag, [this](){ startImpl(); });
  }

  void stop() {
    std::call_once(m_stopped_flag, [this](){ stopImpl(); });
  }

protected:

  void initImpl();
  void startImpl();
  void stopImpl();

private:

  Module() {
    init();
  }

  std::once_flag m_inited_flag {}, m_started_flag {}, m_stopped_flag {};
};

}  // namespace iqlogger
