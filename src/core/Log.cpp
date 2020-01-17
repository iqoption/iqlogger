//
// Log.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "spdlog/sinks/rotating_file_sink.h"

#include "Log.h"

using namespace iqlogger;

bool Logger::init(std::string const& filename, std::string const& level, uint64_t size, size_t count, bool async)
{
    try
    {
        if(async)
        {
            m_logger = spdlog::rotating_logger_mt<spdlog::async_factory>("iqlogger", filename, size, count);
        }
        else
        {
            m_logger = spdlog::rotating_logger_mt("iqlogger", filename, size, count); 
        }

        m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [PID=%P] [TID=%t] [%n] [%l] %v");

        m_logger->flush_on(spdlog::level::from_str(level));
        m_logger->set_level(spdlog::level::from_str(level));

        return true;
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return false;
    }
}

Logger::InnerLoggerPtr iqlogger::Logger::logger() const {
    return m_logger;
}

