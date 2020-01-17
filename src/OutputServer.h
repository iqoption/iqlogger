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

#include "core/Singleton.h"
#include "outputs/Output.h"
#include "config/Config.h"

namespace iqlogger {

    class OutputServer : public Singleton<OutputServer>, public TaskInterface {

        friend class Singleton<OutputServer>;

    public:

        virtual ~OutputServer();

        virtual void start() override;
        virtual void stop() override;
//        void restart() override;

    protected:

        explicit OutputServer();

        size_t m_thread_num;
        std::vector<outputs::OutputPtr> m_outputs;

        std::vector<std::thread> m_threads;

        metrics::atomic_metric_t m_send_counter;

        static outputs::OutputInterface::message_ptr_buffer_t& getMessagePtrBuffer()
        {
            static thread_local outputs::OutputInterface::message_ptr_buffer_t message_buffer;
            return message_buffer;
        }
    };

    using OutputServerPtr = std::shared_ptr<OutputServer>;
}
