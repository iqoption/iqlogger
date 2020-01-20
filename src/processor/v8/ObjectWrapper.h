//
// ObjectWrapper.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "Engine.h"

namespace iqlogger::processor::V8 {

class ObjectWrapper
{
public:
  explicit ObjectWrapper(v8::Isolate* isolate);
  ~ObjectWrapper() = default;

  template<typename T>
  v8::Local<v8::Object> wrapObject(T* object) const;

  template<typename T>
  static T* unwrapObject(v8::Local<v8::Object> obj) {
    v8::Local<v8::External> field = v8::Local<v8::External>::Cast(obj->GetInternalField(0));
    void* ptr = field->Value();
    return static_cast<T*>(ptr);
  }

private:
  v8::Isolate* m_isolate;
};

}  // namespace iqlogger::processor
