//
// CheckConfig.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include <regex>

#include "config/Config.h"

using namespace iqlogger;
using namespace iqlogger::config;

template<>
bool SourceConfigCheckImpl<InputType::TAIL>::operator()(const SourceConfig& sourceConfig) const
{
    if(auto startmsg_regex = sourceConfig.getParam<std::string>("startmsg_regex"); startmsg_regex)
    {
        try
        {
            DEBUG("Checking startmsg_regex " << startmsg_regex.value());
            std::regex re(startmsg_regex.value());
            DEBUG("Checking startmsg_regex success!");
        }
        catch (const std::regex_error& e)
        {
            ERROR("Error checking startmsg_regex (" << e.code() << ") " << e.what());
            return false;
        }
    }

    return true;
}