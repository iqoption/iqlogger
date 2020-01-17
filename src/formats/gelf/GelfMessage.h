//
// GelfMessage.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>

#include "GelfException.h"
#include "formats/MessageInterface.h"
#include "formats/Object.h"
#include "core/Base.h"
#include "core/Log.h"

namespace iqlogger::formats::gelf {

    class GelfMessage : public MessageInterface
    {
        struct gelf
        {
            std::string     version;
            std::string     host;
            std::string     short_message;
            std::string     full_message;
            double          timestamp;
            unsigned char   level;
            Object          additionalFields;

        private:

            static constexpr double min_timestamp   = 1546300800; // @TODO 1 января 2019 г.


        public:

            gelf();
            explicit gelf(std::string_view message);
            std::string dump() const;
        };

        gelf            m_gelf;
        endpoint_t      m_endpoint;

    public:

        using SourceT = endpoint_t;

        GelfMessage();

        template <class T, typename = std::enable_if_t<std::is_constructible_v<std::string, std::decay_t<T>>>>
        explicit GelfMessage(T&& message) :
            m_gelf(decompress(std::forward<T>(message)))
        {
            TRACE("GelfMessage::GelfMessage(std::string&& message)");
        }

        template <class T, typename = std::enable_if_t<std::is_same<std::decay_t<T>, endpoint_t>::value>>
        explicit GelfMessage(std::string&& message, T&& endpoint) :
            m_gelf(decompress(std::move(message))),
            m_endpoint(std::forward<T>(endpoint))
        {
            TRACE("GelfMessage::GelfMessage(std::string&& message, T&& endpoint)");
            //TRACE("Construct GELF Message: " << message << " from " << m_endpoint); @TODO Lazy load and save string
        }

        GelfMessage(GelfMessage&&) noexcept = default;

        template<class T, typename = std::enable_if_t<std::is_constructible<ObjectVariants, std::decay_t<T>>::value>>
        void setField(const std::string& key, T&& value)
        {
            if(key.size())
            {
                if(key == "version")
                {
                    if constexpr (std::is_constructible_v<std::string, std::decay_t<decltype(value)>>)
                    {
                        m_gelf.version = value;
                    }
                    else
                    {
                        WARNING("GELF version MUST be a string!");
                    }

                }
                else if(key == "host")
                {
                    if constexpr (std::is_constructible_v<std::string, std::decay_t<decltype(value)>>)
                    {
                        m_gelf.host = value;
                    }
                    else
                    {
                        WARNING("GELF host MUST be a string!");
                    }
                }
                else if(key == "short_message")
                {
                    if constexpr (std::is_constructible_v<std::string, std::decay_t<decltype(value)>>)
                    {
                        m_gelf.short_message = value;
                    }
                    else
                    {
                        WARNING("GELF short_message MUST be a string!");
                    }
                }
                else if(key == "full_message")
                {
                    if constexpr (std::is_constructible_v<std::string, std::decay_t<decltype(value)>>)
                    {
                        m_gelf.full_message = value;
                    }
                    else
                    {
                        WARNING("GELF full_message MUST be a string!");
                    }
                }
                else if(key == "timestamp")
                {
                    if constexpr (std::is_constructible_v<double, std::decay_t<decltype(value)>>)
                    {
                        m_gelf.timestamp = value;
                    }
                    else if constexpr (std::is_same_v<std::string, std::decay_t<decltype(value)>>)
                    {
                        m_gelf.timestamp = std::stod(value);
                    }
                    else
                    {
                        WARNING("GELF timestamp MUST be a UNIX time!");
                    }
                }
                else if(key == "level")
                {
                    if constexpr (std::is_constructible_v<unsigned char, std::decay_t<decltype(value)>>)
                    {
                        m_gelf.level = value;
                    }
                    else if constexpr (std::is_same_v<std::string, std::decay_t<decltype(value)>>)
                    {
                        m_gelf.level = std::stoi(value);
                    }
                    else
                    {
                        WARNING("GELF level MUST be a short int!");
                    }
                }
                else
                {
                    ObjectVariants v(value);
                    m_gelf.additionalFields.emplace((key[0] == '_') ? key : '_' + key, std::forward<T>(value));
                }
            }
        }

        std::string exportMessage() const override
        {
            return m_gelf.dump();
        }

        inline static double getCurrentTime();
        inline static const std::string& getHostname();

        std::string exportMessage2Json() const override
        {
            return m_gelf.dump();
        }

        endpoint_t getSource() const
        {
            return m_endpoint;
        }

        ~GelfMessage();

    private:

        // @FIXME
        template<class T, typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>>>
        std::string decompress(T&& buffer)
        {
            if (buffer.size() <= 2) throw GelfException("Gelf Message length less than 2 bytes!");

            if ((buffer[0] == '\x1f' && buffer[1] == '\x8b') // gzip magic
                || (buffer[0] == '\x78' && buffer[1] == '\x01') // zlib magic 0
                || (buffer[0] == '\x78' && buffer[1] == '\x9c') // zlib magic 1
                || (buffer[0] == '\x78' && buffer[1] == '\xda') // zlib magic 2
                ) {
                try {

                    constexpr std::streamsize z_buffer_size = boost::iostreams::default_device_buffer_size;

                    std::stringstream compressed(buffer);
                    std::stringstream decompressed;

                    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;

                    if (buffer[0] == '\x1f')
                        out.push(boost::iostreams::gzip_decompressor(boost::iostreams::gzip::default_window_bits,
                                                                     z_buffer_size));

                    if (buffer[0] == '\x78')
                        out.push(boost::iostreams::zlib_decompressor(boost::iostreams::zlib::default_window_bits,
                                                                     z_buffer_size));

                    out.push(compressed);
                    boost::iostreams::copy(out, decompressed);

                    return decompressed.str();
                }
                catch (boost::iostreams::gzip_error &e) {
                    std::ostringstream oss;
                    oss << "Gzip error: " << e.what() << ". Error code: " << e.error() << " (" << e.zlib_error_code()
                        << ")";
                    throw GelfUncompressError(oss.str());
                }
                catch (boost::iostreams::zlib_error &e) {
                    std::ostringstream oss;
                    oss << "Zlib error: " << e.what() << ". Error code: " << e.error();
                    throw GelfUncompressError(oss.str());
                }
            }

            return buffer;
        }

        inline static bool isSystemField(const std::string& key);
    };
}


