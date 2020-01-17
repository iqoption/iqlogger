//
// Engine.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Engine.h"
#include "ProcessorException.h"

using namespace iqlogger;
using namespace iqlogger::processor;

Engine::Engine()
{
    TRACE("Engine::Engine()");
    DEBUG("Initialize V8");

    v8::V8::InitializeICUDefaultLocation("./iqlogger");
    v8::V8::InitializeExternalStartupData("./iqlogger");
    m_platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(m_platform.get());
    v8::V8::Initialize();

    DEBUG("Initialize V8 complete");
}

Engine::~Engine()
{
    TRACE("Engine::~Engine()");
    DEBUG("Destroy V8");

    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();

    DEBUG("Destroy V8 complete");
}

v8::Platform* Engine::getPlatformPtr() const
{
    return m_platform.get();
}

bool Engine::checkScript(const std::string& script_source) const
{
    try
    {
        v8::Isolate::CreateParams create_params;

        create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        v8::Isolate* isolate = v8::Isolate::New(create_params);

        {
            v8::Isolate::Scope isolate_scope(isolate);
            v8::HandleScope handle_scope(isolate);
            v8::Local<v8::Context> context = v8::Context::New(isolate);
            v8::Context::Scope context_scope(context);

            v8::TryCatch try_catch(isolate);

            {
                std::string message_source = "(" + script_source + ")";

                v8::Local<v8::String> source;
                if (!v8::String::NewFromUtf8(isolate, message_source.c_str(), v8::NewStringType::kNormal).ToLocal(&source))
                {
                    std::stringstream oss;
                    oss << "Can't create JS script from: " << message_source;
                    throw ProcessorException(oss.str());
                }

                v8::Local<v8::Script> script;
                if (!v8::Script::Compile(context, source).ToLocal(&script))
                {
                    if(try_catch.HasCaught())
                    {
                        processException("Can't create JS script: ", isolate, &try_catch);
                    }
                    else
                    {
                        throw ProcessorException("Something strange...");
                    }
                }

                v8::Local<v8::Value> result;
                if (!script->Run(context).ToLocal(&result))
                {
                    if(try_catch.HasCaught())
                    {
                        processException("Can't run JS script: ", isolate, &try_catch);
                    }
                    else
                    {
                        throw ProcessorException("Something strange...");
                    }
                }

                while (v8::platform::PumpMessageLoop(getPlatformPtr(), isolate)) continue;

                if(!result->IsFunction())
                {
                    throw ProcessorException("Compiled script is not a function");
                }

                DEBUG("Processor script check done");
            }
        }

        isolate->Dispose();
        delete create_params.array_buffer_allocator;
    }
    catch(const std::exception &e)
    {
        ERROR("Exception due script check: " << e.what());
        return false;
    }

    DEBUG("Processor script checked (compiled) succeed");
    return true;
}

void Engine::processException(const std::string& msg, v8::Isolate* isolate, v8::TryCatch* try_catch) const
{
    std::stringstream oss;
    oss << msg << std::endl;

    v8::HandleScope handle_scope(isolate);
    v8::String::Utf8Value exception(isolate, try_catch->Exception());

    v8::Local<v8::Message> message = try_catch->Message();
    if (message.IsEmpty())
    {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
        oss << *exception << std::endl;
    }
    else
    {
        // Print (filename):(line number): (message).
        v8::String::Utf8Value filename(isolate, message->GetScriptOrigin().ResourceName());
        v8::Local<v8::Context> context(isolate->GetCurrentContext());

        int linenum = message->GetLineNumber(context).FromJust();

        oss << *filename << ":" << linenum << " " <<*exception << std::endl;

        // Print line of source code.
        v8::String::Utf8Value sourceline(isolate,
                                         message->GetSourceLine(context).ToLocalChecked());

        oss << *sourceline << std::endl;

        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn(context).FromJust();
        for (int i = 0; i < start; i++)
        {
            oss << " ";
        }
        int end = message->GetEndColumn(context).FromJust();
        for (int i = start; i < end; i++)
        {
            oss << "^";
        }

        oss << std::endl;

        v8::Local<v8::Value> stack_trace_string;

        if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) &&
            stack_trace_string->IsString() &&
            v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0)
        {
            v8::String::Utf8Value stack_trace(isolate, stack_trace_string);
            oss << *stack_trace << std::endl;
        }

        throw ProcessorException(oss.str());
    }
}