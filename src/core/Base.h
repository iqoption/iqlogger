//
// Base.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <boost/asio.hpp>

#include <variant>

namespace iqlogger {

template<class InternetProtocol>
using endpoint = boost::asio::ip::basic_endpoint<InternetProtocol>;
using endpoint_t = std::variant<endpoint<boost::asio::ip::tcp>, endpoint<boost::asio::ip::udp>>;
}  // namespace iqlogger

std::ostream& operator<<(std::ostream& os, const iqlogger::endpoint_t& v);
