//
// IsolateScope.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "IsolateScope.h"

#include "Console.h"
#include "core/Log.h"
#include "processor/ProcessorException.h"

using namespace iqlogger;
using namespace iqlogger::processor;
using namespace iqlogger::processor::V8;

IsolateScope::IsolateScope(v8::Isolate* isolate, ScriptMap scriptMap) :
    m_enginePtr(Engine::getInstance()),
    m_isolate(isolate),
    m_wrapper(m_isolate),
    m_isolate_scope(m_isolate),
    m_handle_scope(m_isolate),
    m_context(v8::Context::New(m_isolate)),
    m_context_scope(m_context),
    m_try_catch(m_isolate),
    m_console(m_wrapper.wrapObject(Console::getInstance().get())) {
  TRACE("IsolateScope::IsolateScope()");

  if (auto result = m_context->Global()->Set(
          m_context, v8::String::NewFromUtf8(m_isolate, "console", v8::NewStringType::kNormal).ToLocalChecked(),
          m_console);
      !result.ToChecked()) {
    if (m_try_catch.HasCaught()) {
      processException("Can't initialize console: ");
    } else {
      throw ProcessorException("Something strange...");
    }
  }

  for (const auto& it : scriptMap) {
    DEBUG("Processor init script for index: " << it.first);

    v8::Local<v8::String> source;
    if (!v8::String::NewFromUtf8(m_isolate, it.second.c_str(), v8::NewStringType::kNormal).ToLocal(&source)) {
      std::stringstream oss;
      oss << "Can't create JS script from: " << it.second;
      throw ProcessorException(oss.str().c_str());
    }

    v8::Local<v8::Script> script;
    if (!v8::Script::Compile(m_context, source).ToLocal(&script)) {
      if (m_try_catch.HasCaught()) {
        processException("Can't create JS script: ");
      } else {
        throw ProcessorException("Something strange...");
      }
    }

    v8::Local<v8::Value> result;
    if (!script->Run(m_context).ToLocal(&result)) {
      if (m_try_catch.HasCaught()) {
        processException("Can't run JS script: ");
      } else {
        throw ProcessorException("Something strange...");
      }
    }

    while (v8::platform::PumpMessageLoop(m_enginePtr->getPlatformPtr(), m_isolate))
      continue;

    if (!result->IsFunction()) {
      throw ProcessorException("Compiled script is not a function");
    }

    m_functions[it.first] = v8::Handle<v8::Function>::Cast(result);
  }
  DEBUG("IsolateScope init done");
}

UniqueMessagePtr IsolateScope::process(ProcessorRecordPtr processorRecordPtr) {
  TRACE("IsolateScope::process()");

  const auto& message = processorRecordPtr->first;
  const auto& scriptIndex = processorRecordPtr->second;

  if (scriptIndex > m_functions.size()) {
    throw ProcessorException("Error script index");
  }

  auto jsObject = makeJsObject(message);

  v8::Local<v8::Value> args[] = {jsObject};
  v8::Local<v8::Value> result;

  if (!m_functions[scriptIndex]->Call(m_context, m_context->Global(), 1, args).ToLocal(&result)) {
    if (m_try_catch.HasCaught()) {
      processException("JavaScript Error: ");
    } else {
      throw ProcessorException("Something strange...");
    }
  }

  UniqueMessagePtr resultMessage;

  if (result->IsBoolean() && result->IsFalse()) {
    DEBUG("Processor return false value, ignoring...");
    resultMessage = nullptr;
  } else if (result->IsString()) {
    v8::String::Utf8Value str(m_isolate, result->ToString(m_context).ToLocalChecked());
    resultMessage = makeMessageObject(*str);
  } else if (result->IsObject()) {
    v8::Local<v8::Value> newJsonString;
    if (!v8::JSON::Stringify(m_context, result).ToLocal(&newJsonString)) {
      if (m_try_catch.HasCaught()) {
        processException("JavaScript Error: ");
      } else {
        throw ProcessorException("Something strange...");
      }
    } else if (!newJsonString->IsString()) {
      std::stringstream oss;
      v8::String::Utf8Value utf8(m_isolate, newJsonString);
      oss << "Result from processor script: " << *utf8 << " is not a string!";
      throw ProcessorException(oss.str());
    }
    v8::String::Utf8Value str(m_isolate, newJsonString);
    resultMessage = makeMessageObject(*str);
  } else {
    std::stringstream oss;
    v8::String::Utf8Value utf8(m_isolate, result);
    oss << "Result from processor script " << *utf8 << " is not object!";
    throw ProcessorException(oss.str());
  }

  while (v8::platform::PumpMessageLoop(m_enginePtr->getPlatformPtr(), m_isolate))
    continue;

  return resultMessage;
}

IsolateScope::~IsolateScope() {
  TRACE("IsolateScope::~IsolateScope()");
}

void IsolateScope::processException(const std::string& msg) const {
  std::stringstream oss;
  oss << msg << std::endl;

  v8::HandleScope handle_scope(m_isolate);
  v8::String::Utf8Value exception(m_isolate, m_try_catch.Exception());

  v8::Local<v8::Message> message = m_try_catch.Message();
  if (message.IsEmpty()) {
    // V8 didn't provide any extra information about this error; just
    // print the exception.
    oss << *exception << std::endl;
  } else {
    // Print (filename):(line number): (message).
    v8::String::Utf8Value filename(m_isolate, message->GetScriptOrigin().ResourceName());
    v8::Local<v8::Context> context(m_isolate->GetCurrentContext());

    int linenum = message->GetLineNumber(context).FromJust();

    oss << *filename << ":" << linenum << " " << *exception << std::endl;

    // Print line of source code.
    v8::String::Utf8Value sourceline(m_isolate, message->GetSourceLine(context).ToLocalChecked());

    oss << *sourceline << std::endl;

    // Print wavy underline (GetUnderline is deprecated).
    int start = message->GetStartColumn(context).FromJust();
    for (int i = 0; i < start; i++) {
      oss << " ";
    }
    int end = message->GetEndColumn(context).FromJust();
    for (int i = start; i < end; i++) {
      oss << "^";
    }

    oss << std::endl;

    v8::Local<v8::Value> stack_trace_string;

    if (m_try_catch.StackTrace(context).ToLocal(&stack_trace_string) && stack_trace_string->IsString() &&
        v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
      v8::String::Utf8Value stack_trace(m_isolate, stack_trace_string);
      oss << *stack_trace << std::endl;
    }

    throw ProcessorException(oss.str());
  }
}

v8::Local<v8::Object> IsolateScope::makeJsObject(const UniqueMessagePtr& messagePtr) const {
  return std::visit([this](auto& message) { return m_wrapper.wrapObject(&message); }, *messagePtr);
}

UniqueMessagePtr IsolateScope::makeMessageObject(std::string&& message) {
  auto processorMessage = ProcessorMessage(std::move(message));
  return std::make_unique<Message>(std::move(processorMessage));
}