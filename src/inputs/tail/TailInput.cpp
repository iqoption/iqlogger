//
// TailInput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "TailInput.h"
#include "SavePositionServer.h"

using namespace iqlogger;
using namespace iqlogger::inputs::tail;

TailInput::TailInput(const config::SourceConfig& sourceConfig) :
        Input::Input(sourceConfig)
{
    TRACE("TailInput::TailInput()");

    if(auto path = sourceConfig.getParam<std::string>("path"); path)
    {
        m_path = path.value();

        if(m_path[m_path.size() - 1] == '/')
        {
            DEBUG("Correcting... " << m_path);
            m_path.pop_back();
        }

        INFO("Listen to path " << m_path);
    }
    else
    {
        std::ostringstream oss;
        oss << "Path for input " << m_name << " not specified!";
        throw Exception(oss.str());
    }

    auto followOnly = sourceConfig.getParam<bool>("follow_only_mode").value_or(default_follow_only_mode);
    INFO("Follow mode for input " << m_name << " is " << std::to_string(followOnly));

    auto saveState = sourceConfig.getParam<bool>("save_state").value_or(false);

    if(followOnly)
    {
        INFO("Save state for input " << m_name << " is " << std::to_string(saveState));

        if(saveState)
        {
            std::ostringstream oss;
            oss << "Input " << m_name << " Follow only mode and save state mode are incompatible!";
            throw Exception(oss.str());
        }
    }
    else
    {
        saveState = sourceConfig.getParam<bool>("save_state").value_or(true);
        INFO("Save state for input " << m_name << " is " << std::to_string(saveState));
    }

    if(auto startmsg_regex = sourceConfig.getParam<std::string>("startmsg_regex"); startmsg_regex)
    {
        // Разделитель - регулярное выражение
        m_delimiterType = DelimiterType::REGEX;
        m_monitorPtr = std::make_shared<TailMonitor>(m_name, m_inputQueuePtr, boost::regex {startmsg_regex.value(), boost::match_default | boost::match_nosubs}, m_path, followOnly, saveState);
    }
    else
    {
        m_delimiterType = DelimiterType::NEWLINE;
        m_monitorPtr = std::make_shared<TailMonitor>(m_name, m_inputQueuePtr, std::nullopt, m_path, followOnly, saveState);
    }

    m_read_timeout = sourceConfig.getParam<unsigned short>("read_timeout").value_or(default_read_timeout);
}

TailInput::~TailInput()
{
    TRACE("TailInput::~TailInput()");
}

void TailInput::start()
{
    TRACE("TailInput::start()");
    INFO("Start tail input");

    Input::start();

    INFO("Start Save Position Server");
    SavePositionServer::getInstance()->start();

    INFO("Start Inotify Server");
    InotifyServer::getInstance()->start();

    INFO("Start timer thread");
    m_timerThread = std::thread([this]() {

        TRACE("Tail Timer Thread started");

        if(m_read_timeout)
        {
            while(!isStopped())
            {
                m_monitorPtr->flush();
                std::this_thread::sleep_for(std::chrono::seconds(m_read_timeout));
            }
        }
    });

    INFO("Start monitor threads...");
    for (size_t i = 0; i < m_thread_num; ++i)
    {
        m_threads.emplace_back(std::thread([this]()
        {
            try
            {
                while(!isStopped())
                {
                    if(!m_monitorPtr->run())
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            catch (const std::exception &e)
            {
                CRITICAL("Tail Input thread running exited with " << e.what());
            }
        }));
    }
}

void TailInput::stop()
{
    TRACE("TailInput::stop()");

    // @TODO Не очень понятно, как стопить отдельный input (Возможно перенести stop в деструктор)
    SavePositionServer::getInstance()->stop();
    InotifyServer::getInstance()->stop();

    Input::stop();

    TRACE("Join timer thread... ");

    if(m_timerThread.joinable())
        m_timerThread.join();

    TRACE("Join monitor threads... ");
    for(auto& t : m_threads)
    {
        if(t.joinable())
            t.join();
    }
}
