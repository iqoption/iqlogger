//
// TaskInterface.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <any>
#include <atomic>

namespace iqlogger {

class TaskInterface
{
public:
  TaskInterface() = default;

  virtual ~TaskInterface() = default;

  virtual void init(std::any config) final {
    if (!m_inited.exchange(true)) {
      try {
        initImpl(std::move(config));
      } catch (const std::bad_any_cast& e) {
        throw std::logic_error(e.what());
      }
    } else {
      throw std::logic_error("Init already inited task!");
    }
  }

  virtual void start() final {
    if (!m_started.exchange(true)) {
      if (isInited()) {
        startImpl();
      } else {
        throw std::logic_error("Start not inited task!");
      }
    } else {
      throw std::logic_error("Start already started task!");
    }
  }

  virtual void stop() final {
    if (m_started.exchange(false)) {
      stopImpl();
    } else {
      throw std::logic_error("Stop not started task!");
    }
  }

  virtual bool isInited() const final { return m_inited.load(); }
  virtual bool isRunning() const final { return m_started.load(); }

protected:
  virtual void initImpl(std::any config) = 0;
  virtual void startImpl() = 0;
  virtual void stopImpl() = 0;

private:
  std::atomic_bool m_inited{false};
  std::atomic_bool m_started{false};
};
}