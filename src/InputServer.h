//
// InputServer.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <boost/asio/io_service.hpp>
#include <core/TaskInterface.h>

#include "config/Config.h"
#include "core/Singleton.h"
#include "inputs/InputInterface.h"

namespace iqlogger {

class InputServer : public Singleton<InputServer>, public TaskInterface
{
  friend class Singleton<InputServer>;

public:
  virtual ~InputServer();

protected:
  void initImpl(std::any) override;
  void startImpl() override;
  void stopImpl() override;

private:
  InputServer();

  std::vector<inputs::InputPtr> m_inputs;

  size_t m_thread_num;
  std::vector<std::thread> m_threads;

  metrics::atomic_metric_t m_receive_counter;
};

using InputServerPtr = std::shared_ptr<InputServer>;
}  // namespace iqlogger
