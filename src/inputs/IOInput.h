//
// Input.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <boost/asio.hpp>

#include "Input.h"

namespace iqlogger { namespace inputs {

    template<class T>
    class IOInput : public Input<T> {

    protected:

        boost::asio::io_service m_io_service;

    public:

        using Input<T>::Input;

        virtual void start() override
        {
            Input<T>::start();

            TRACE("IOInput::start()");

            // Start io threads

            for (size_t i = 0; i < Input<T>::m_thread_num; ++i)
            {
                Input<T>::m_threads.emplace_back(std::thread([this]() {

                    try
                    {
                        m_io_service.run();
                    }
                    catch (const std::exception &e)
                    {
                        CRITICAL("Input Server thread running exited with " << e.what());
                    }
                }));
            }
        }

        virtual void stop() override
        {
            TRACE("Input stop m_io_service");
            m_io_service.stop();

            Input<T>::stop();

            TRACE("Join input io threads... ");
            for(auto& t : Input<T>::m_threads)
            {
                if(t.joinable())
                    t.join();
            }
        }
    };
}}


