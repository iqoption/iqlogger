//
// ConsoleWrapper.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "ConsoleWrapper.h"

#include "Console.h"
#include "ObjectWrapper.h"
#include "processor/ProcessorException.h"

using namespace iqlogger::processor::V8;

void ConsoleWrapper::log(const v8::FunctionCallbackInfo<v8::Value>& args) {

  for(int i = 0; i < args.Length(); ++i) {
    if(args[i]->IsObject() || args[i]->IsArray()) {

      v8::Local<v8::Value> strObject;

      if (!v8::JSON::Stringify(args.GetIsolate()->GetCurrentContext(), args[i]).ToLocal(&strObject)) {
        std::stringstream oss;
        v8::String::Utf8Value str(args.GetIsolate(), args[i]);
        oss << "Error sringify object: " << *str;
        throw ProcessorException(oss.str());
      } else if (!strObject->IsString()) {
        std::stringstream oss;
        v8::String::Utf8Value str(args.GetIsolate(), strObject);
        oss << "Error sringify object: " << *str;
        throw ProcessorException(oss.str());
      }
      v8::String::Utf8Value str(args.GetIsolate(), strObject);
      Console::getInstance()->log(*str);
    } else {
      v8::String::Utf8Value str(args.GetIsolate(), args[i]);
      Console::getInstance()->log(*str);
    }
  }
}

void ConsoleWrapper::error(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::String::Utf8Value str(args.GetIsolate(), args[0]);
  Console::getInstance()->error(*str);
}

template<>
v8::Local<v8::Object> ObjectWrapper::wrapObject<Console>(Console* object) const {
  v8::EscapableHandleScope handle_scope(m_isolate);

  v8::Local<v8::ObjectTemplate> class_t;
  v8::Local<v8::ObjectTemplate> raw_t = v8::ObjectTemplate::New(m_isolate);
  raw_t->SetInternalFieldCount(1);
  raw_t->Set(v8::String::NewFromUtf8(m_isolate, "log", v8::NewStringType::kNormal).ToLocalChecked(),
             v8::FunctionTemplate::New(m_isolate, ConsoleWrapper::log));
  raw_t->Set(v8::String::NewFromUtf8(m_isolate, "error", v8::NewStringType::kNormal).ToLocalChecked(),
             v8::FunctionTemplate::New(m_isolate, ConsoleWrapper::error));
  class_t = v8::Local<v8::ObjectTemplate>::New(m_isolate, raw_t);
  // create instance
  v8::Local<v8::Object> result = class_t->NewInstance(m_isolate->GetCurrentContext()).ToLocalChecked();
  // create wrapper
  v8::Local<v8::External> ptr = v8::External::New(m_isolate, object);
  result->SetInternalField(0, ptr);
  return handle_scope.Escape(result);
}