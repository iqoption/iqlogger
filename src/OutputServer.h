//
// OutputServer.h
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
#include "outputs/Output.h"

namespace iqlogger {

class OutputServer : public Singleton<OutputServer>, public TaskInterface
{
  friend class Singleton<OutputServer>;

public:
  virtual ~OutputServer();

protected:

  void initImpl(std::any) override;
  void startImpl() override;
  void stopImpl() override;

private:
  explicit OutputServer();

  static outputs::OutputInterface::message_ptr_buffer_t& getMessagePtrBuffer() {
    static thread_local outputs::OutputInterface::message_ptr_buffer_t message_buffer;
    return message_buffer;
  }

  size_t m_thread_num;
  std::vector<outputs::OutputPtr> m_outputs;
  std::vector<std::thread> m_threads;
  metrics::atomic_metric_t m_send_counter;
};

using OutputServerPtr = std::shared_ptr<OutputServer>;
}  // namespace iqlogger
