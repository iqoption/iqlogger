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

#include "core/Singleton.h"
#include "inputs/InputInterface.h"
#include "config/Config.h"

namespace iqlogger {

    class InputServer : public Singleton<InputServer>, public TaskInterface {

        friend class Singleton<InputServer>;

        std::vector<inputs::InputPtr> m_inputs;

        // Потоки для внутренней работы сервера (перекладывание из очередей + валидация)
        size_t m_thread_num;
        std::vector<std::thread> m_threads;

        metrics::atomic_metric_t m_receive_counter;

        InputServer();

    public:

        virtual ~InputServer();

        virtual void start() override;
        virtual void stop() override;
//        void restart() override;
    };

    using InputServerPtr = std::shared_ptr<InputServer>;
}


