//
// Atomic.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <atomic>
#include <new>

namespace iqlogger::utils::atomic {

// libc++ 8.0 and later define __cpp_lib_hardware_interference_size but don't actually implement it
#if __cplusplus < 201703L || !(defined(__cpp_lib_hardware_interference_size) && !defined(_LIBCPP_VERSION))

constexpr std::size_t hardware_destructive_interference_size = alignof(std::max_align_t);
constexpr auto hardware_constructive_interference_size = hardware_destructive_interference_size;

#else

using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;

#endif

template<typename T>
struct aligned_atomic_t {
  alignas(hardware_destructive_interference_size) std::atomic<T> value;
};

}  // namespace iqlogger::utils::atomic
