//
// Log.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <frozen/string.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "core/Singleton.h"
#include "spdlog/async.h"
#include "spdlog/spdlog.h"

namespace iqlogger {

using LogLevel = spdlog::level::level_enum;

class Logger : public Singleton<Logger>
{
public:
  using InnerLoggerPtr = std::shared_ptr<spdlog::logger>;

  virtual ~Logger() {}
  bool init(std::string const& filename, std::string const& level, uint64_t size, size_t count, bool async = false);

  InnerLoggerPtr logger() const;

private:
  Logger() noexcept = default;
  friend class Singleton<Logger>;

  InnerLoggerPtr m_logger = nullptr;
};
}  // namespace iqlogger

#define GET_LOGGER_MACRO(_1, _2, NAME, ...) NAME

#define LOG_PRINT(logger, lLevel, logMethod, msg)                                          \
  {                                                                                        \
    if (logger) {                                                                          \
      if (logger->should_log(spdlog::level::lLevel)) {                                     \
        try {                                                                              \
          std::ostringstream oss;                                                          \
          oss << msg;                                                                      \
          logger->logMethod(oss.str());                                                    \
        } catch (const std::exception& e) {                                                \
          std::cerr << "Failed on logging a message." << std::endl                         \
                    << std::endl                                                           \
                    << "Level: lLevel"                                                     \
                    << "Cause: " << e.what() << std::endl;                                 \
        }                                                                                  \
      }                                                                                    \
    } else {                                                                               \
      constexpr frozen::string llevel{#lLevel};                                            \
      if constexpr (llevel == frozen::string("warn") || llevel == frozen::string("err") || \
                    llevel == frozen::string("critical"))                                  \
        std::cerr << msg << std::endl;                                                     \
      else if constexpr (llevel == frozen::string("info"))                                 \
        std::cout << msg << std::endl;                                                     \
    }                                                                                      \
  }

#define TRACE_LOGGER(logger, msg) LOG_PRINT(logger, trace, trace, msg)
#define TRACE_DEF_LOGGER(msg) LOG_PRINT(iqlogger::Logger::getInstance()->logger(), trace, trace, msg)

#define DEBUG_LOGGER(logger, msg) LOG_PRINT(logger, debug, debug, msg)
#define DEBUG_DEF_LOGGER(msg) LOG_PRINT(iqlogger::Logger::getInstance()->logger(), debug, debug, msg)

#define INFO_LOGGER(logger, msg) LOG_PRINT(logger, info, info, msg)
#define INFO_DEF_LOGGER(msg) LOG_PRINT(iqlogger::Logger::getInstance()->logger(), info, info, msg)

#define WARNING_LOGGER(logger, msg) LOG_PRINT(logger, warn, warn, msg)
#define WARNING_DEF_LOGGER(msg) LOG_PRINT(iqlogger::Logger::getInstance()->logger(), warn, warn, msg)

#define ERROR_LOGGER(logger, msg) LOG_PRINT(logger, err, error, msg)
#define ERROR_DEF_LOGGER(msg) LOG_PRINT(iqlogger::Logger::getInstance()->logger(), err, error, msg)

#define CRITICAL_LOGGER(logger, msg) LOG_PRINT(logger, critical, critical, msg)
#define CRITICAL_DEF_LOGGER(msg) LOG_PRINT(iqlogger::Logger::getInstance()->logger(), critical, critical, msg)

#define TRACE(...) GET_LOGGER_MACRO(__VA_ARGS__, TRACE_LOGGER, TRACE_DEF_LOGGER)(__VA_ARGS__)
#define DEBUG(...) GET_LOGGER_MACRO(__VA_ARGS__, DEBUG_LOGGER, DEBUG_DEF_LOGGER)(__VA_ARGS__)
#define INFO(...) GET_LOGGER_MACRO(__VA_ARGS__, INFO_LOGGER, INFO_DEF_LOGGER)(__VA_ARGS__)
#define WARNING(...) GET_LOGGER_MACRO(__VA_ARGS__, WARNING_LOGGER, WARNING_DEF_LOGGER)(__VA_ARGS__)
#define ERROR(...) GET_LOGGER_MACRO(__VA_ARGS__, ERROR_LOGGER, ERROR_DEF_LOGGER)(__VA_ARGS__)
#define CRITICAL(...) GET_LOGGER_MACRO(__VA_ARGS__, CRITICAL_LOGGER, CRITICAL_DEF_LOGGER)(__VA_ARGS__)
