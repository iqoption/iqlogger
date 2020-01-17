//
// Server.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Server.h"

using namespace iqlogger::inputs::gelf::udp;
using namespace iqlogger::formats::gelf;

void Server::udp_session::process(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    auto buffer_begin = m_buffer.cbegin(), buffer_end = m_buffer.cend();
    std::string buff(buffer_begin, buffer_begin +
                                   std::min((std::size_t) std::max(0l, std::distance(buffer_begin, buffer_end)), bytes_transferred));

    TRACE("Receive " << bytes_transferred << " b from " << m_remote_endpoint << ": `" << buff << "` EC: " << error.message());

    try
    {
        if (buff.size() <= 2) throw GelfException("Message length less than 2 bytes!");

        if (buff[0] == '\x1e' && buff[1] == '\x0f')
        {
            auto chunkPtr = std::make_shared<GelfChunk>(std::move(buff), m_remote_endpoint);

            TRACE("Chunked GELF: ID=`" << chunkPtr->getMessageId() << "` " << chunkPtr->getSequenceNumber() << " / " << chunkPtr->getSequenceCount());

            if(!m_chunkQueuePtr->enqueue(chunkPtr))
            {
                ERROR("Chunk queue is full... Dropping...");
            }
        }
        else
        {
            if(!m_queuePtr->enqueue(std::make_unique<Record<Gelf>>(std::move(buff), m_remote_endpoint)))
            {
                ERROR("Gelf UDP Input queue is full... Dropping...");
            }
        }
    }
    catch (const GelfException& e)
    {
        WARNING("Buffer decode error: " << e.what() << " from " << m_remote_endpoint);
        DEBUG("Receive " << bytes_transferred << " b: `" << buff << "`");
    }
}