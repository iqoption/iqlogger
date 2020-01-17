//
// Base.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <variant>
#include <boost/asio.hpp>

namespace iqlogger {

    // Need to noexcept move constructor (absent in boost::asio::ip::basic_endpoint)
    template<class InternetProtocol>
    class endpoint : public boost::asio::ip::basic_endpoint<InternetProtocol>
    {
    public:

        using basic_endpoint = boost::asio::ip::basic_endpoint<InternetProtocol>;
        using basic_endpoint::basic_endpoint;

        endpoint(endpoint&& other) noexcept
        : basic_endpoint(std::move(other))
        {
        }

        endpoint(const endpoint& other) = default;

        endpoint& operator=(endpoint&& other) noexcept
        {
            basic_endpoint::operator=(std::move(other));
            return *this;
        }

        endpoint(const basic_endpoint& other)
            : basic_endpoint::basic_endpoint(other)
        {
        }
    };

    using endpoint_t = std::variant<endpoint<boost::asio::ip::tcp>, endpoint<boost::asio::ip::udp>>;

    static_assert(std::is_nothrow_assignable_v<endpoint<boost::asio::ip::tcp>, endpoint<boost::asio::ip::tcp>&&>
                  && std::is_nothrow_constructible_v<endpoint<boost::asio::ip::tcp>, endpoint<boost::asio::ip::tcp>&&>);
}

std::ostream& operator<<(std::ostream& os, const iqlogger::endpoint_t& v);
