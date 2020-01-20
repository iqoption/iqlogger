//
// Output.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>

#include "MessageQueue.h"
#include "config/Config.h"
#include "core/TaskInterface.h"
#include "metrics/Metrics.h"

namespace iqlogger::outputs {

class OutputInterface;
using OutputPtr = std::unique_ptr<OutputInterface>;

class OutputInterface : public TaskInterface
{
public:
  using message_ptr_buffer_t = std::array<UniqueMessagePtr, max_queue_bulk_size>;

  explicit OutputInterface(config::DestinationConfig destinationConfig);
  virtual ~OutputInterface() = default;

  static OutputPtr instantiate(const config::DestinationConfig& destinationConfig,
                               iqlogger::metrics::atomic_metric_t& total_outputs_send_counter);

  virtual bool importMessages(message_ptr_buffer_t::const_iterator it, size_t count) const = 0;

protected:

  void initImpl(std::any) override;
  void stopImpl() override;

protected:
  template<config::OutputType... Outputs>
  static OutputPtr instantiateImpl(const config::DestinationConfig& destinationConfig,
                                   iqlogger::metrics::atomic_metric_t& total_outputs_send_counter) {
    auto checker = utils::types::make_switch(
        [&]() -> OutputPtr {
          std::stringstream oss;
          oss << "Unknown type " << destinationConfig.type;
          throw Exception(oss.str());
        },
        std::make_pair(Outputs,
                       [&]() { return instantiateOutput<Outputs>(destinationConfig, total_outputs_send_counter); })...);
    return checker(destinationConfig.type);
  }

  template<config::OutputType Output>
  static OutputPtr instantiateOutput(const config::DestinationConfig& destinationConfig,
                                     iqlogger::metrics::atomic_metric_t& total_outputs_send_counter);

  std::string m_name;
  config::OutputType m_type;
  size_t m_thread_num;
  std::vector<std::thread> m_threads;
  boost::asio::io_service m_io_service;
  std::chrono::seconds m_timeout;
};
}  // namespace iqlogger::outputs
