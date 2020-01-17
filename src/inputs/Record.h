//
// Record.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>
#include <type_traits>

#include "Message.h"
#include "MessageQueue.h"
#include "core/Log.h"

namespace iqlogger::inputs {

    template<class T>
    class Record;

    template<class T>
    using RecordPtr = std::unique_ptr<Record<T>>;

    template<class T>
    class Record {

        using RecordDataT   = typename T::RecordDataT;
        using MessageT      = typename T::MessageT;

        template <typename U, void* fake = nullptr>
        struct Source
        {
            U source;

            template<class W>
            explicit Source(W&& s) : source(std::forward<W>(s)) {};
        };

        template <void* fake> class Source<void, fake> {};

        using SourceT   = Source<typename T::SourceT>;

        RecordDataT m_data;
        SourceT     m_source;

    public:

        template <typename = std::enable_if_t<std::is_same<typename T::SourceT, void>::value>>
        explicit Record(RecordDataT&& data) : m_data(std::move(data))
        {
            TRACE("inputs::Record::Record()");
        }

        template <
            class U,
            typename = std::enable_if_t<std::is_constructible<typename T::SourceT, std::decay_t<U>>::value>,
            typename = std::enable_if_t<!std::is_same<typename T::SourceT, void>::value>
        >
        explicit Record(RecordDataT&& data, U&& source) : m_data(std::move(data)), m_source(std::forward<U>(source))
        {
            TRACE("inputs::Record::Record()");
        }

        Record(const Record&) = delete;
        Record& operator =(const Record&) = delete;
        explicit Record(Record&&) noexcept = default;

        ~Record()
        {
            TRACE("inputs::Record::~Record()");
        }

        UniqueMessagePtr exportMessage()
        {
            if constexpr (std::is_same<typename T::SourceT, void>::value)
            {
                MessageT message (std::move(m_data));
                return std::make_unique<Message>(std::move(message));
            }
            else
            {
                MessageT message (std::move(m_data), m_source.source);
                return std::make_unique<Message>(std::move(message));
            }
        }
    };

    template<class T>
    using RecordQueue       = MessageQueue<RecordPtr<T>>;

    template<class T>
    using RecordQueuePtr    = std::shared_ptr<RecordQueue<T>>;
}



