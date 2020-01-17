//
// Connection.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include <utility>
#include <vector>

#include "Connection.h"
#include "ConnectionManager.h"
#include "RequestHandler.h"

using namespace iqlogger::stats::http;

Connection::Connection(boost::asio::io_service &io_service, ConnectionManager& manager, RequestHandler& handler)
  : m_socket(io_service),
    m_connectionManager(manager),
    m_requestHandler(handler)
{
}

void Connection::start()
{
    do_read();
}

void Connection::stop()
{
    m_socket.close();
}

void Connection::do_read()
{
  auto self(shared_from_this());
    m_socket.async_read_some(boost::asio::buffer(m_buffer),
      [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
      {
        if (!ec)
        {
            RequestParser::result_type result;
            std::tie(result, std::ignore) = m_request_parser.parse(
                m_request, m_buffer.data(), m_buffer.data() + bytes_transferred);

            if (result == RequestParser::result_type::good)
            {
                m_requestHandler.handle_request(m_request, m_reply);
                do_write();
            }
            else if (result == RequestParser::result_type::bad)
            {
                m_reply = Reply::stock_reply(Reply::status_type::bad_request);
                do_write();
            }
            else
            {
                do_read();
            }
        }
        else if (ec != boost::asio::error::operation_aborted)
        {
            m_connectionManager.stop(shared_from_this());
        }
      });
}

void Connection::do_write()
{
    auto self(shared_from_this());

    boost::asio::async_write(m_socket, m_reply.to_buffers(),
      [this, self](boost::system::error_code ec, std::size_t)
      {
            if (!ec)
            {
                boost::system::error_code ignored_ec;
                m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
            }

            if (ec != boost::asio::error::operation_aborted)
            {
                m_connectionManager.stop(shared_from_this());
            }
      });
}
