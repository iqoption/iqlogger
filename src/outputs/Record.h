//
// Record.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>

#include "Message.h"
#include "MessageQueue.h"
#include "core/Log.h"

namespace iqlogger::outputs {

    template<class T>
    class Record;

    template<class T>
    using RecordPtr = std::unique_ptr<Record<T>>;

    template<class T>
    class Record {

        using DataRecordT   = typename T::DataT;
        using MessageT      = typename T::MessageT;
        using DataExportT   = typename T::DataExportT;

        DataRecordT m_data;

    public:

        template <class U>
        explicit Record(const U& data);

        ~Record()
        {
            TRACE("outputs::Record::~Record()");
        }

        Record(const Record&) = delete;
        Record& operator =(const Record&) = delete;

        DataExportT exportRecord() const
        {
            return m_data;
        }
    };

    template<class T>
    using RecordQueue       = MessageQueue<RecordPtr<T>>;

    template<class T>
    using RecordQueuePtr    = std::shared_ptr<RecordQueue<T>>;
}



