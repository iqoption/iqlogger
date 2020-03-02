//
// GelfOutput.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "GelfOutput.h"
#include "outputs/Record.h"
#include "outputs/gelf/Gelf.h"

using namespace iqlogger::outputs;
using namespace iqlogger::outputs::gelf::tcp;

GelfOutput::GelfOutput(config::DestinationConfig destinationConfig, metrics::atomic_metric_t& total_outputs_send_counter) :
    Output::Output(destinationConfig)
{
    if(auto hosts = destinationConfig.getParam<std::vector<std::string>>("hosts"); hosts && !hosts.value().empty())
    {
        m_upstreams = hosts.value();
    }
    else
    {
        std::ostringstream oss;
        oss << "Hosts are empty in dst " << m_name;
        throw config::ConfigException(oss.str());
    }

    if(auto port = destinationConfig.getParam<unsigned short>("port"); port && port.value() != 0)
    {
        m_port = port.value();
    }

    if(auto outputBufferSize = destinationConfig.getParam<std::size_t>("output_buffer_size"); outputBufferSize && outputBufferSize.value() != 0)
    {
        m_outputBufferSize = outputBufferSize.value();
    }
    else
    {
        m_outputBufferSize = 8 * 4096;
    }

    if(auto maxMessageSize = destinationConfig.getParam<std::size_t>("max_message_size"); maxMessageSize && maxMessageSize.value() != 0)
    {
        m_maxMessageSize = maxMessageSize.value();
    }
    else
    {
        m_maxMessageSize = 8 * 1024 * 1024;
    }

    m_outputThreadCallback = [this, &total_outputs_send_counter](const std::string& upstream) {

        // @FIXME Refactor from duty lambdas 2 normal class

        constexpr std::size_t queue_bulk = 1000;

        boost::system::error_code error;
        std::stringstream buff;

        INFO("Running iqlogger output thread to upstream " << upstream << ":" << m_port << " Timeout: " << m_timeout.count() << " s.");

        boost::asio::ip::tcp::socket socket(m_io_service);

        auto run = [this, &socket]() {

            m_io_service.restart();
            m_io_service.run_for(m_timeout);

            if (!m_io_service.stopped())
            {
                socket.close();
                m_io_service.run();
            }
        };

        auto connect = [this, &socket, &error, upstream, port = m_port, &run]() {

            DEBUG("Connect to " << upstream << ":" << port);

            boost::asio::ip::tcp::resolver resolver(m_io_service);

            try
            {
                boost::asio::ip::tcp::resolver::results_type endpoints =
                        resolver.resolve(upstream, std::to_string(port));

                TRACE("Connect...");
                boost::asio::async_connect(socket, endpoints, [&error](const boost::system::error_code& ec,
                                                                       const boost::asio::ip::tcp::endpoint& endpoint) {

                    TRACE("Connect to " << endpoint << ". Result: " << ec.message());
                    error = ec;

                });

                run();

                TRACE("Set options...");

                if(!error)
                {
                    boost::asio::socket_base::keep_alive keep_alive_option(true);
                    socket.set_option(keep_alive_option);
                }
            }
            catch (const std::runtime_error& e)
            {
                WARNING("Error: " << e.what());
                error = boost::asio::error::host_not_found_try_again;
            }
        };

        auto reconnect = [&socket, &error, &connect]() {

            DEBUG("Reconnect...");

            if(socket.is_open())
            {
                socket.close();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            connect();
        };

        connect();

        while(error && !isStopped())
        {
            error.clear();
            reconnect();
        }

        DEBUG("Connection established...");

        auto flush = [this, &error, &socket, &connect, &reconnect, &run](std::string const& str) {

            TRACE("flushing buffer...");

            error.clear();

            do
            {
                if(error)
                {
                    ERROR("Send failed: " << error.message() << " Trying to reconnect...");
                    reconnect();
                }

                TRACE("Writing " << str.length() << " bytes");
                boost::asio::async_write(socket, boost::asio::buffer(str), [&error](const boost::system::error_code &ec, std::size_t bytes) {
                    TRACE("Sent " << bytes << " bytes. Result: " << ec.message());
                    error = ec;
                });
                run();
                TRACE("Write result: " << error.message());
            }
            while(error && !isStopped());
        };

        std::array<RecordPtr<Gelf>, queue_bulk> bulk;
        std::size_t count;

        while(!isStopped())
        {
            count = m_outputQueuePtr->try_dequeue_bulk(bulk.begin(), queue_bulk);

            if(count)
            {
                buff.str("");
                std::size_t j = 0;
                std::size_t current_buff_len = 0;

                while(j < count)
                {
                    auto str = bulk[j]->exportRecord();

                    if(str.size() > m_maxMessageSize)
                    {
                        WARNING("Message size of record: `" << str << "` is out of max message size " << m_maxMessageSize << " bytes. Dropped!");
                    }
                    else
                    {
                        TRACE("Record: " << str);
                        buff << str << '\0';
                        current_buff_len += str.length();
                    }

                    if(current_buff_len >= m_outputBufferSize)
                    {
                        flush(buff.str());
                        buff.str("");
                        current_buff_len = 0;
                    }

                    ++j;
                }

                if(current_buff_len)
                {
                    flush(buff.str());
                }

                m_send_counter += count;
                total_outputs_send_counter += count;
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    };
}

void GelfOutput::start()
{
    Output::start();

    for (std::size_t i = 0; i < m_thread_num; ++i) {

        m_threads.emplace_back(std::thread([this, upstream = m_upstreams[i % m_upstreams.size()]](){
            m_outputThreadCallback(upstream);
        }));
    }
}

void GelfOutput::stop() {

    Output::stop();

    for(auto& t : m_threads)
    {
        if(t.joinable())
            t.join();
    }
}


