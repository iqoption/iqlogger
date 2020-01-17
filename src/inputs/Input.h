//
// Input.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "metrics/MetricsController.h"
#include "InputInterface.h"
#include "MainMessageQueue.h"

#include "metrics/Metrics.h"

namespace iqlogger::inputs {

    template<class T>
    class Input : public InputInterface {

    protected:

        static std::array<RecordPtr<T>, max_queue_bulk_size>& getRecordInputBuffer()
        {
            static thread_local std::array<RecordPtr<T>, max_queue_bulk_size> record_input_buffer;
            return record_input_buffer;
        }

        static std::array<UniqueMessagePtr, max_queue_bulk_size>& getMessageOutputBuffer()
        {
            static thread_local std::array<UniqueMessagePtr, max_queue_bulk_size> message_output_buffer;
            return message_output_buffer;
        }

        RecordQueuePtr<T> m_inputQueuePtr;
        mutable metrics::atomic_metric_t m_receive_counter;

    public:

        explicit Input(const config::SourceConfig& sourceConfig) :
                InputInterface(sourceConfig),
                m_inputQueuePtr {std::make_shared<RecordQueue<T>>("inputs." + m_name + ".queue", sourceConfig.max_queue_size, sourceConfig.overflow_stategy)},
                m_receive_counter (0)
        {
            TRACE("Input::Input()");
        }

        virtual void start() override
        {
            InputInterface::start();

            INFO ("Register metrics for input " << m_name);

            metrics::MetricsController::getInstance()->registerMetric("inputs." + m_name + ".receive_total", [this]() {
                return m_receive_counter.load();
            });
        }

        virtual unsigned long importMessages() const override
        {
            auto mainMessageQueuePtr = MainMessageQueue::getInstance();

            auto& record_input_buffer = getRecordInputBuffer();
            auto& message_output_buffer = getMessageOutputBuffer();

            std::size_t count = m_inputQueuePtr->try_dequeue_bulk(record_input_buffer.begin()),
                        j = 0,
                        k = 0;

            if(count)
            {
                TRACE("Import " << count << " messages from " << m_name);

                for (; j < count; ++j)
                {
                    try
                    {
                        // @TODO Some refactoring
                        message_output_buffer[k] = record_input_buffer[j]->exportMessage();
                        ++k;
                    }
                    catch(const Exception& e)
                    {
                        WARNING("Error construct message: " << e.what());
                    }
                }

                if(k)
                {
                    m_receive_counter += k;

#ifdef IQLOGGER_WITH_PROCESSOR
                    if(hasProcessor())
                    {
                        TRACE("Send " << count << " messages to processor " << m_name);

                        if (!process(std::make_move_iterator(message_output_buffer.begin()), k))
                        {
                            ERROR("Processor queue is full... Dropping...");
                        }
                    }
                    else
                    {
#endif
                        if (!mainMessageQueuePtr->enqueue_bulk(
                                std::make_move_iterator(message_output_buffer.begin()), k)) {
                            ERROR("Main queue is full... Dropping...");
                        }
#ifdef IQLOGGER_WITH_PROCESSOR
                    }
#endif
                }
            }

            return k;
        }
    };
}

