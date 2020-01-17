//
// Base.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Base.h"

using namespace iqlogger;

std::ostream& operator<<(std::ostream& os, const endpoint_t& v)
{
    std::visit([&os](auto&& arg) {
        os << arg;
    }, v);
    return os;
}

