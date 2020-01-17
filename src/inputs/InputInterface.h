//
// InputInterface.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>

#include "config/Config.h"
#include "Exception.h"
#include "core/TaskInterface.h"
#include "MessageQueue.h"
#include "Record.h"

#ifdef IQLOGGER_WITH_PROCESSOR
#include <processor/Processor.h>
#endif

namespace iqlogger::inputs {

    class InputInterface;
    using InputPtr = std::unique_ptr<InputInterface>;

    class InputInterface : public TaskInterface {

    protected:

        std::string m_name;
        config::InputType m_type;

        size_t m_thread_num;
        std::vector<std::thread> m_threads;

#ifdef IQLOGGER_WITH_PROCESSOR
        std::optional<iqlogger::processor::ProcessorScriptIndex> m_processorScriptIndex;
#endif

    public:

        explicit InputInterface(const config::SourceConfig& sourceConfig);
        virtual ~InputInterface() = default;

        static InputPtr instantiate(const config::SourceConfig& sourceConfig);

        virtual unsigned long importMessages() const = 0;

#ifdef IQLOGGER_WITH_PROCESSOR
        inline bool hasProcessor() const
        {
            return m_processorScriptIndex.has_value();
        }

        template<typename It>
        inline bool process(It itemFirst, size_t count) const
        {
            if(hasProcessor())
            {
                return iqlogger::processor::Processor::getInstance()->process_bulk(itemFirst, count, m_processorScriptIndex.value());
            }

            return false;
        }
#endif
    };
}


