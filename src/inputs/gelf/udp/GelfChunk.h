//
// GelfChunk.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <iostream>
#include <boost/asio/ip/udp.hpp>

#include "concurrentqueue/concurrentqueue.h"

namespace iqlogger::inputs::gelf::udp {

//        Prepend the following structure to your GELF message to make it chunked:
//
//        Chunked GELF magic bytes - 2 bytes: 0x1e 0x0f
//        Message ID - 8 bytes: Must be the same for every chunk of this message. Identifying the whole message and is used to reassemble the chunks later. Generate from millisecond timestamp + hostname for example.
//        Sequence number - 1 byte: The sequence number of this chunk. Starting at 0 and always less than the sequence count.
//        Sequence count - 1 byte: Total number of chunks this message has.
//        All chunks MUST arrive within 5 seconds or the server will discard all already arrived and still arriving chunks. A message MUST NOT consist of more than 128 chunks.

    class GelfChunk {

    protected:

        std::string m_messageId;
        size_t m_sequenceNumber;
        size_t m_sequenceCount;

        std::string m_chunk;

        boost::asio::ip::udp::endpoint m_source;

    public:

        // @TODO
        explicit GelfChunk(std::string const& chunk, const boost::asio::ip::udp::endpoint& source);

        std::string getMessageId() const;
        size_t getSequenceNumber() const;
        size_t getSequenceCount() const;
        std::string getChunk() const;
        boost::asio::ip::udp::endpoint getSource() const;
    };

    using GelfChunkPtr  = std::shared_ptr<GelfChunk>;
    using ChunkQueue    = moodycamel::ConcurrentQueue<GelfChunkPtr>;
    using ChunkQueuePtr = std::shared_ptr<ChunkQueue>;
}

