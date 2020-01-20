//
// IsolateScope.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "Engine.h"
#include "ObjectWrapper.h"
#include "processor/ProcessorTypes.h"

namespace iqlogger::processor::V8 {

class IsolateScope
{
public:
  IsolateScope(v8::Isolate* isolate, ScriptMap scriptMap);
  ~IsolateScope();

  UniqueMessagePtr process(ProcessorRecordPtr processorRecordPtr);

private:

  v8::Local<v8::Object> makeJsObject(const UniqueMessagePtr& messagePtr) const;

  static UniqueMessagePtr makeMessageObject(std::string&& message);

  void processException(const std::string& msg) const;

private:
  EnginePtr m_enginePtr;

  v8::Isolate* m_isolate;

  ObjectWrapper m_wrapper;

  v8::Isolate::Scope m_isolate_scope;
  v8::HandleScope m_handle_scope;
  v8::Local<v8::Context> m_context;
  v8::Context::Scope m_context_scope;
  v8::TryCatch m_try_catch;
  v8::Local<v8::Object> m_console;
  std::unordered_map<ProcessorScriptIndex, v8::Handle<v8::Function>> m_functions;
};

}  // namespace iqlogger::processor
