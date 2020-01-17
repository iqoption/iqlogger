//
// Processor.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>

#include <tbb/concurrent_vector.h>

#include "core/Singleton.h"
#include "core/Log.h"
#include "core/TaskInterface.h"
#include "MessageQueue.h"
#include "Engine.h"

namespace iqlogger::processor {

    using ProcessorScriptIndex = unsigned short;

    class Processor : public Singleton<Processor>, public TaskInterface
    {
        using ScriptMap                 = tbb::concurrent_vector<std::string>;
        using ProcessorRecord           = std::pair<UniqueMessagePtr, ProcessorScriptIndex>;
        using ProcessorRecordPtr        = std::unique_ptr<ProcessorRecord>;
        using ProcessorRecordQueue      = MessageQueue<ProcessorRecordPtr>;
        using ProcessorRecordQueuePtr   = std::shared_ptr<ProcessorRecordQueue>;

        using ProcessorRecordPtrBuffer  = std::array<ProcessorRecordPtr, max_queue_bulk_size>;

        friend class Singleton<Processor>;
        Processor();

        EnginePtr m_enginePtr;

        size_t m_thread_num;
        std::vector<std::thread> m_threads;

        ScriptMap m_scriptSources;

        ProcessorRecordQueuePtr m_processQueuePtr;

        static ProcessorRecordPtrBuffer& getProcessorRecordPtrBuffer()
        {
            static thread_local ProcessorRecordPtrBuffer record_buffer;
            return record_buffer;
        }

    public:

        virtual ~Processor();

        template<typename It>
        bool process_bulk(It itemFirst, size_t count, ProcessorScriptIndex processorScriptIndex)
        {
            auto& record_buffer = getProcessorRecordPtrBuffer();

            for(size_t i = 0; i < count; i++)
            {
                record_buffer [i] = std::make_unique<ProcessorRecord>(std::make_pair(std::move(*(itemFirst + i)), processorScriptIndex));
            }

            auto result = m_processQueuePtr->enqueue_bulk(std::make_move_iterator(record_buffer.begin()), count);
            return result;
        }

        ProcessorScriptIndex addProcessor(const std::string& script_source);

        virtual void start() override;
        virtual void stop() override;
//        void restart() override;
    };

    using ProcessorPtr = std::shared_ptr<Processor>;
}


