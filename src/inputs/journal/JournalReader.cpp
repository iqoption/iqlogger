//
// JournalReader.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "JournalReader.h"

using namespace iqlogger::inputs::journal;

JournalReader::JournalReader(RecordQueuePtr<Journal> queuePtr, boost::asio::io_service &io_service, units_filter_t units_filter) :
    m_strand(io_service),
    m_queuePtr(queuePtr),
    m_units(units_filter)
{
    TRACE("JournalReader::JournalReader()");

    int error_code = sd_journal_open(&m_journal, SD_JOURNAL_LOCAL_ONLY);
    if (error_code < 0)
    {
        std::stringstream oss;
        oss << "Failed to open journal: " << strerror(-error_code);
        throw Exception(oss.str());
    }

    m_journal_inotify_fd = sd_journal_get_fd(m_journal);

    if (m_journal_inotify_fd < 0)
    {
        std::stringstream oss;
        oss << "Failed to get journal FD: " << strerror(-m_journal_inotify_fd);
        throw Exception(oss.str());
    }

    if(m_units)
    {
        INFO("Journal filtered by UNITS: ");
        for(const auto& unit_name : m_units.value())
        {
            INFO("Add filter: " << unit_name);
            std::string filter = "_SYSTEMD_UNIT=" + unit_name;
            error_code = sd_journal_add_match(m_journal, filter.c_str(), 0);

            if (error_code < 0)
            {
                std::stringstream oss;
                oss << "Failed add filter to journal: " << strerror(-error_code);
                throw Exception(oss.str());
            }
        }

        if(m_units.value().size() > 1)
        {
            error_code = sd_journal_add_disjunction(m_journal);

            if (error_code < 0)
            {
                std::stringstream oss;
                oss << "Failed add filter to journal: " << strerror(-error_code);
                throw Exception(oss.str());
            }
        }
    }

    error_code = sd_journal_seek_tail(m_journal);
    if (error_code < 0)
    {
        std::stringstream oss;
        oss << "Failed to seek to tail of journal: " << strerror(-error_code);
        throw Exception(oss.str());
    }

    // For some reason, fetching previous then next avoids getting an initial
    // huge chunk of records.  If one calls sd_journal_seek_tail() and then
    // calls sd_journal_next() we receive the huge chunk.
    error_code = sd_journal_previous(m_journal);
    if (error_code < 0)
    {
        std::stringstream oss;
        oss << "Failed to get previous journal entry: " << strerror(-error_code);
        throw Exception(oss.str());
    }

    error_code = sd_journal_next(m_journal);

    if (error_code < 0)
    {
        std::stringstream oss;
        oss << "Failed to seek journal: " << strerror(-error_code);
        throw Exception(oss.str());
    }

    m_journal_descriptor = std::make_unique<boost::asio::posix::stream_descriptor>(io_service, m_journal_inotify_fd);

    async_read();
}

JournalReader::~JournalReader()
{
    TRACE("JournalReader::~JournalReader()");
    sd_journal_close(m_journal);
}

void JournalReader::read_journal()
{
    int error_code;

    while ((error_code = sd_journal_next(m_journal)) == 1)
    {
        JournalRecord values;
        const void* data;
        size_t length;
        SD_JOURNAL_FOREACH_DATA(m_journal, data, length) {
            std::string line(static_cast<const char*>(data), length);
            auto equal_pos = line.find('=');
            if (equal_pos == std::string::npos)
                continue;

            values.insert(std::make_pair(line.substr(0, equal_pos), line.substr(equal_pos+1)));
        }

        uint64_t usec;
        sd_journal_get_realtime_usec(m_journal, &usec);

        JournalMessage message(values, usec);

        process(std::move(message));
    }

    if (error_code < 0)
    {
        WARNING("Error calling sd_journal_next: " << strerror(error_code));
    }
}

void JournalReader::handle_receive(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (error)
    {
        WARNING("Error polling journal file descriptor: " << error.message().c_str());
    }
    else
    {
        TRACE("Fetch " << bytes_transferred << " bytes from Journal ID");

        int error_code = sd_journal_process(m_journal);
        switch (error_code)
        {
            case SD_JOURNAL_NOP:
                break;
            case SD_JOURNAL_APPEND:
                read_journal();
                break;
            case SD_JOURNAL_INVALIDATE:
                //TODO: How to handle?
                break;
            default:
                WARNING("Unexpected return code from sd_journal_process: " << error_code);
        }
    }

    async_read();
}

void JournalReader::async_read()
{
    auto handler = [this](const boost::system::error_code &error, std::size_t bytes_transferred) {
        handle_receive(error, bytes_transferred);
    };

    m_journal_descriptor->async_read_some(boost::asio::null_buffers(), m_strand.wrap(handler));
}

void JournalReader::process(JournalMessage&& journal_message)
{
    try
    {
        if(!m_queuePtr->enqueue(std::make_unique<Record<Journal>>(std::move(journal_message))))
        {
            ERROR("Journal Input queue is full... Dropping...");
        }
    }
    catch (const Exception &e)
    {
        WARNING("Journal buffer decode error: " << e.what());
    }
}

