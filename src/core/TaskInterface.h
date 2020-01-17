//
// TaskInterface.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <atomic>
#include <stdexcept>

namespace iqlogger {

    class TaskInterface {

    public:

        TaskInterface() : m_stoped{false}, m_started(false) {};
        virtual ~TaskInterface() {
            if(isStarted())
                stop();
        };

        virtual void start() {
            if(!isStarted()) m_started = true;
            else throw std::logic_error("Start already started task!");
        }

        virtual void stop() {
            if(isStarted()) m_stoped = true;
            else throw std::logic_error("Stop not started task!");
        }

        virtual bool isStopped() final {
            return m_stoped.load();
        }

        virtual bool isStarted() final {
            return m_started.load();
        }

    private:

        std::atomic_bool m_stoped;
        std::atomic_bool m_started;

    };
}