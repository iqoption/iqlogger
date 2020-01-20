//
// ObjectWrapper.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "ObjectWrapper.h"

#include "processor/ProcessorMessage.h"

using namespace iqlogger::processor;
using namespace iqlogger::processor::V8;

ObjectWrapper::ObjectWrapper(v8::Isolate* isolate) : m_isolate(isolate) {
  TRACE("ObjectWrapper::ObjectWrapper()");
}

template<>
v8::Local<v8::Object> ObjectWrapper::wrapObject<std::string>(std::string* object) const {
  return v8::Local<v8::Object>::Cast(
      v8::String::NewFromUtf8(m_isolate, object->c_str(), v8::NewStringType::kNormal).ToLocalChecked());
}

template<>
v8::Local<v8::Object> ObjectWrapper::wrapObject<ProcessorMessage>(ProcessorMessage*) const {
  WARNING("Loop processor message into input message queue...");
  return v8::Local<v8::Object>::Cast(v8::Object::New(m_isolate));
}