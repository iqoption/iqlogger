//
// JournalReader.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <systemd/sd-journal.h>
#include <boost/asio.hpp>

#include "MessageQueue.h"
#include "inputs/Input.h"

#include "Journal.h"

using namespace iqlogger::formats::journal;

namespace iqlogger::inputs::journal {

    class JournalReader {

        using units_filter_t = std::optional<std::vector<std::string>>;

        sd_journal *m_journal;
        int m_journal_inotify_fd;
        std::unique_ptr<boost::asio::posix::stream_descriptor> m_journal_descriptor;

        boost::asio::io_service::strand m_strand;
        RecordQueuePtr<Journal> m_queuePtr;

        units_filter_t m_units;

        void handle_receive(const boost::system::error_code &error, std::size_t bytes_transferred);
        void read_journal();
        void async_read();

        void process(JournalMessage&& journal_message);

    public:

        JournalReader(RecordQueuePtr<Journal> queuePtr, boost::asio::io_service &io_service, units_filter_t units_filter);
        ~JournalReader();

    };

    using JournalReaderPtr = std::unique_ptr<JournalReader>;
}


