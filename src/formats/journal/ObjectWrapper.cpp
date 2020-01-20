//
// ObjectWrapper.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "JournalMessage.h"
#include "config/input/InputConfig.h"

#ifdef IQLOGGER_WITH_PROCESSOR

#include "processor/ProcessorException.h"
#include "processor/v8/ObjectWrapper.h"

using namespace iqlogger::processor;
using namespace iqlogger::formats::journal;
using namespace iqlogger::processor::V8;

template<>
v8::Local<v8::Object> ObjectWrapper::wrapObject<JournalMessage>(JournalMessage* message) const {
  auto jsonString =
      v8::String::NewFromUtf8(m_isolate, message->exportMessage2Json().c_str(), v8::NewStringType::kNormal)
          .ToLocalChecked();

  v8::Local<v8::Value> result;
  if (!v8::JSON::Parse(m_isolate->GetCurrentContext(), jsonString).ToLocal(&result)) {
    std::stringstream oss;
    v8::String::Utf8Value str(m_isolate, jsonString);
    oss << "Can't parse JSON Object in Processor: " << *str;
    throw ProcessorException(oss.str());
  } else if (!result->IsObject()) {
    std::stringstream oss;
    v8::String::Utf8Value str(m_isolate, jsonString);
    oss << "Can't create JSON Object in Processor: " << *str;
    throw ProcessorException(oss.str());
  }

  return v8::Local<v8::Object>::Cast(result);
}

#endif
