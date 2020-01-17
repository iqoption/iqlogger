//
// Processor.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "config/ConfigManager.h"
#include "MainMessageQueue.h"
#include "Processor.h"
#include "ProcessorException.h"

using namespace iqlogger;
using namespace iqlogger::processor;

Processor::Processor() :
    TaskInterface::TaskInterface(),
    m_enginePtr { Engine::getInstance() },
    m_thread_num {config::ConfigManager::getInstance()->getConfig()->processorConfig.threads},
    m_processQueuePtr {std::make_shared<ProcessorRecordQueue>("processor.queue")}
{
    TRACE("Processor::Processor()");
}

ProcessorScriptIndex Processor::addProcessor(const std::string& script_source)
{
    TRACE("Processor::addProcessor()");
    INFO("Add processor script: " << script_source);
    ProcessorScriptIndex index = std::distance(m_scriptSources.begin(), m_scriptSources.push_back(script_source));
    DEBUG("Processor index is: " << index);
    return index;
}

void Processor::start()
{
    TRACE("Processor::start()");

    TaskInterface::start();

    DEBUG("Retrieve scripts from config...");

    for (size_t i = 0; i < m_thread_num; ++i)
    {
        m_threads.emplace_back(std::thread([i, this]() {

            INFO("Register processor " << i << " thread metric...");

            // @TODO Если этот тред хлопнется раньше, то метрики будут обращаться по битой ссылке
            uint64_t heap_size = 0;

            metrics::MetricsController::getInstance()->registerMetric("processor." + std::to_string(i) + ".heap_size", [&heap_size]() {
                return heap_size;
            });

            try
            {
                auto mainMessageQueuePtr = MainMessageQueue::getInstance();

                v8::Isolate::CreateParams create_params;

                create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
                v8::Isolate* isolate = v8::Isolate::New(create_params);

                {
                    v8::Isolate::Scope isolate_scope(isolate);
                    v8::HandleScope handle_scope(isolate);
                    v8::Local<v8::Context> context = v8::Context::New(isolate);
                    v8::Context::Scope context_scope(context);

                    v8::TryCatch try_catch(isolate);

                    DEBUG("Script map: " << m_scriptSources.size());

                    std::vector<v8::Handle<v8::Function>> functions(m_scriptSources.size());

                    {
                        for(ProcessorScriptIndex index = 0; index < m_scriptSources.size(); ++index)
                        {
                            DEBUG("Processor init script for index: " << index);

                            std::string message_source = "(" + m_scriptSources[index] + ")";

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
                                    m_enginePtr->processException("Can't create JS script: ", isolate, &try_catch);
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
                                    m_enginePtr->processException("Can't run JS script: ", isolate, &try_catch);
                                }
                                else
                                {
                                    throw ProcessorException("Something strange...");
                                }
                            }

                            while (v8::platform::PumpMessageLoop(m_enginePtr->getPlatformPtr(), isolate)) continue;

                            if(!result->IsFunction())
                            {
                                throw ProcessorException("Compiled script is not a function");
                            }

                            TRACE("Set script function index " << index << " Is Function: " << result->IsFunction());
                            functions[index] = v8::Handle<v8::Function>::Cast(result);
                        }

                        DEBUG("Processor init done");

                        std::size_t count, j, k;

                        // @TODO
                        std::array<ProcessorRecordPtr, max_queue_bulk_size> record_input_buffer;
                        std::array<UniqueMessagePtr, max_queue_bulk_size> message_output_buffer;

                        while(!isStopped())
                        {
                            count = m_processQueuePtr->try_dequeue_bulk(record_input_buffer.begin());

                            if(count)
                            {
                                TRACE("Import " << count << " messages in processor thread...");

                                v8::HandleScope h_scope(isolate);

                                for (j = k = 0; j < count; ++j)
                                {
                                    try
                                    {
                                        // @FIXME
                                        auto& messagePtr  = record_input_buffer[j]->first;
                                        auto processorScriptIndex = record_input_buffer[j]->second;

                                        auto function = functions[processorScriptIndex];

                                        v8::Local<v8::Object> jsObject;

                                        std::visit([this, &jsObject, isolate, &try_catch, &context](const auto& mess) {

                                            using T = std::decay_t<decltype(mess)>;

                                            v8::Local<v8::String> jsonString;

                                            if constexpr (std::is_same_v<T, std::string>)
                                            {
                                                TRACE(mess);
                                                jsonString = v8::String::NewFromUtf8(isolate, mess.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
                                            }
                                            else
                                            {
                                                auto str = mess.exportMessage2Json();
                                                TRACE(str);
                                                jsonString = v8::String::NewFromUtf8(isolate, str.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
                                            }

                                            v8::Local<v8::Value> result;

                                            if(!v8::JSON::Parse(context, jsonString).ToLocal(&result))
                                            {
                                                std::stringstream oss;
                                                v8::String::Utf8Value utf8(isolate, jsonString);
                                                oss << "Can't parse JSON Object in Processor: " << *utf8;

                                                if(try_catch.HasCaught())
                                                {
                                                    m_enginePtr->processException(oss.str(), isolate, &try_catch);
                                                }
                                                else
                                                {
                                                    throw ProcessorException(oss.str());
                                                }
                                            }
                                            else if(!result->IsObject())
                                            {
                                                std::stringstream oss;
                                                v8::String::Utf8Value utf8(isolate, jsonString);
                                                oss << "Can't create JSON Object in Processor: " << *utf8;

                                                if(try_catch.HasCaught())
                                                {
                                                    m_enginePtr->processException(oss.str(), isolate, &try_catch);
                                                }
                                                else
                                                {
                                                    throw ProcessorException(oss.str());
                                                }
                                            }

                                            jsObject = v8::Local<v8::Object>::Cast(result);

                                        }, *messagePtr);

                                        v8::Local<v8::Value> args[] = { jsObject };
                                        v8::Local<v8::Value> result;

                                        if (!function->Call(context, context->Global(), 1, args).ToLocal(&result))
                                        {
                                            if(try_catch.HasCaught())
                                            {
                                                m_enginePtr->processException("JavaScript Error: ", isolate, &try_catch);
                                            }
                                            else
                                            {
                                                throw ProcessorException("Something strange...");
                                            }
                                        }
                                        else if(!result->IsObject())
                                        {
                                            std::stringstream oss;
                                            v8::String::Utf8Value utf8(isolate, result);
                                            oss << "Result from processor script " << *utf8 << " is not object!";
                                            throw ProcessorException(oss.str());
                                        }

                                        v8::Local<v8::Value> newJsonString;

                                        if (!v8::JSON::Stringify(context, result).ToLocal(&newJsonString))
                                        {
                                            if(try_catch.HasCaught())
                                            {
                                                m_enginePtr->processException("JavaScript Error: ", isolate, &try_catch);
                                            }
                                            else
                                            {
                                                throw ProcessorException("Something strange...");
                                            }
                                        }
                                        else if (!newJsonString->IsString())
                                        {
                                            std::stringstream oss;
                                            v8::String::Utf8Value utf8(isolate, newJsonString);
                                            oss << "Result from processor script: " << *utf8 << " is not a string!";
                                            throw ProcessorException(oss.str());
                                        }

                                        v8::String::Utf8Value utf8(isolate, newJsonString);
                                        std::string newMessageJson {*utf8};

                                        TRACE("Return from processor: " << newMessageJson);

                                        UniqueMessagePtr newMessagePtr;

                                        std::visit([&newMessagePtr, json = std::move(newMessageJson)](auto& mess) mutable {

                                            using T = std::decay_t<decltype(mess)>;

                                            if constexpr (std::is_same<T, std::string>::value)
                                            {
                                                newMessagePtr = std::make_unique<Message>(T(std::move(json)));
                                            }
                                            else if constexpr (std::is_same<typename T::SourceT, void>::value)
                                            {
                                                newMessagePtr = std::make_unique<Message>(T(std::move(json)));
                                            }
                                            else
                                            {
                                                newMessagePtr = std::make_unique<Message>(T(std::move(json), std::forward<typename T::SourceT>(mess.getSource())));
                                            }

                                        }, *messagePtr);

                                        message_output_buffer[k] = std::move(newMessagePtr);

                                        ++k;
                                    }
                                    catch(const Exception& e)
                                    {
                                        WARNING("Error process message from: " << e.what());
                                    }

                                    while (v8::platform::PumpMessageLoop(m_enginePtr->getPlatformPtr(), isolate)) continue;
                                }

                                if(k)
                                {
                                    if (!mainMessageQueuePtr->enqueue_bulk(std::make_move_iterator(message_output_buffer.begin()), k))
                                    {
                                        ERROR("Main queue is full... Dropping...");
                                    }
                                }
                            }
                            else
                            {
                                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                            }

                            v8::HeapStatistics stats;
                            isolate->GetHeapStatistics(&stats);
                            //heap_size.store(stats.total_heap_size());
                            heap_size = stats.total_heap_size();
                        }
                    }
                }

                isolate->Dispose();
                delete create_params.array_buffer_allocator;
            }
            catch(const std::exception &e)
            {
                CRITICAL("Exception " << e.what());
                stop();
            }
        }));
    }

    TRACE("Processor::start() <-");
}

void Processor::stop()
{
    TRACE("Processor::stop()");

    TaskInterface::stop();

    TRACE("Join processor threads... ");
    for(auto& t : m_threads)
    {
        if(t.joinable())
            t.join();
    }
}

Processor::~Processor()
{
    TRACE("Processor::~Processor()");

    if(isStarted())
        stop();
}