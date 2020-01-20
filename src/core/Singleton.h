//
// Singleton.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>

namespace iqlogger {

template<class T>
class Singleton
{
public:
  static std::shared_ptr<T> getInstance() {
    static std::shared_ptr<T> instance(new T());
    return instance;
  }

  Singleton() = default;
  ~Singleton() = default;
  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;
};
}  // namespace iqlogger
