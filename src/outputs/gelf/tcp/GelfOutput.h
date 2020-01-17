//
// GelfOutput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include "outputs/Output.h"
#include "outputs/gelf/Gelf.h"

namespace iqlogger::outputs::gelf::tcp {

    class GelfOutput : public Output<Gelf> {

    public:

        explicit GelfOutput(config::DestinationConfig destinationConfig, metrics::atomic_metric_t& total_outputs_send_counter);

        virtual void start() override;
        virtual void stop() override;
//        virtual void restart() override;

    private:

        unsigned short                          m_port;
        std::vector<std::string>                m_upstreams;
        std::function<void(std::string const&)> m_outputThreadCallback;

        std::size_t                             m_outputBufferSize;
        std::size_t                             m_maxMessageSize;

    };

}

