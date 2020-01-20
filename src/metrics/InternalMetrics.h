//
// InternalMetrics.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <sys/resource.h>

#include <filesystem>

#include "core/Log.h"
#include "Metrics.h"
#include "ProcStat.h"

namespace iqlogger::metrics {

class InternalMetrics
{
  int m_pid;
  long m_page_size;
  clock_t m_clock_tick;

public:
  InternalMetrics() : m_pid(getpid()), m_page_size(sysconf(_SC_PAGESIZE)), m_clock_tick(sysconf(_SC_CLK_TCK)) {
    INFO("Internal metrics main PID: " << m_pid);
  }

  ~InternalMetrics() = default;

  template<class RegisterCallbackT>
  bool registerInternalMetrics(RegisterCallbackT registerCallbackFunction) {
    return

        registerCallbackFunction("process.cpu_user_seconds_total",
                                 []() {
                                   struct rusage p;
                                   getrusage(RUSAGE_SELF, &p);
                                   return p.ru_utime.tv_sec;
                                 }) &&

        registerCallbackFunction("process.cpu_system_seconds_total",
                                 []() {
                                   struct rusage p;
                                   getrusage(RUSAGE_SELF, &p);
                                   return p.ru_stime.tv_sec;
                                 }) &&

        registerCallbackFunction("process.resident_memory_bytes",
                                 [this]() {
                                   procstat::procstat p;
                                   get_proc_info(m_pid, &p);
                                   return p.rss * m_page_size;
                                 }) &&

        registerCallbackFunction("process.threads",
                                 [this]() {
                                   procstat::procstat p;
                                   get_proc_info(m_pid, &p);
                                   return p.num_threads;
                                 }) &&
#ifndef __APPLE__
        registerCallbackFunction("process.open_fds",
                                 [this]() {
                                   std::string path = "/proc/" + std::to_string(m_pid) + "/fd/";
                                   return std::distance(std::filesystem::directory_iterator(path),
                                                        std::filesystem::directory_iterator());
                                 }) &&
#endif
        registerCallbackFunction("process.max_fds", []() {
          struct rlimit rlim;
          getrlimit(RLIMIT_NOFILE, &rlim);
          return rlim.rlim_max;
        });
  }
};

}  // namespace iqlogger::metrics
