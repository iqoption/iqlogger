//
// Output.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>

#include "config/Config.h"
#include "core/TaskInterface.h"
#include "MessageQueue.h"
#include "metrics/Metrics.h"

namespace iqlogger::outputs {

    class OutputInterface;
    using OutputPtr = std::unique_ptr<OutputInterface>;

    class OutputInterface : public TaskInterface {

    public:

        using message_ptr_buffer_t = std::array<UniqueMessagePtr, max_queue_bulk_size>;

        explicit OutputInterface(config::DestinationConfig destinationConfig);
        virtual ~OutputInterface() = default;

        static OutputPtr instantiate(config::DestinationConfig destinationConfig, iqlogger::metrics::atomic_metric_t& total_outputs_send_counter);

        virtual bool importMessages(message_ptr_buffer_t::const_iterator it, size_t count) const = 0;

        virtual void start() override;
        virtual void stop() override;

    protected:

        std::string m_name;
        config::OutputType m_type;

        size_t m_thread_num;
        std::vector<std::thread> m_threads;

        boost::asio::io_service m_io_service;

        std::chrono::seconds m_timeout;
    };
}


