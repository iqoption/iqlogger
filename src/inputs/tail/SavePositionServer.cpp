//
// SavePositionServer.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/algorithm/string/replace.hpp>

#include "SavePositionServer.h"

using namespace iqlogger;
using namespace iqlogger::inputs::tail;

SavePositionServer::SavePositionServer()
{
    TRACE("SavePositionServer::SavePositionServer()");
}

void SavePositionServer::start()
{
    TRACE("SavePositionServer::start()");

    std::call_once(m_started_flag, [this](){

        INFO("Start SavePosition Server");
        TaskInterface::start();

        init();
    });
}

void SavePositionServer::stop()
{
    TRACE("SavePositionServer::stop()");

    std::call_once(m_stopped_flag, [this]() {

        INFO("Stop SavePosition Server");

        TaskInterface::stop();

    });
}

SavePositionServer::~SavePositionServer()
{
    TRACE("SavePositionServer::~SavePositionServer()");
}

void SavePositionServer::init()
{
    TRACE("SavePositionServer::init()");

    try
    {
        if(!std::filesystem::exists(data_dir.data()) || !std::filesystem::is_directory(data_dir.data()))
        {
            std::ostringstream oss;
            oss << "Path to store state: " << data_dir.data() << "doesn't exists!";
            throw Exception(oss.str());
        }

        for(auto& p: std::filesystem::directory_iterator(data_dir.data()))
        {
            auto filename = p.path().filename().string();
            auto pos = getCurrentSavedPositionFromFile(p.path());

            DEBUG("Initial position from " << filename << " is " << pos);

            SavedPointersTableInternal::accessor accessor;
            if (m_savedPointersTableInternal.insert(accessor, filename))
            {
                accessor->second = pos;
            }

            std::filesystem::remove(p.path());
        }
    }
    catch(const std::exception& e)
    {
        std::ostringstream oss;
        oss << "Error read from FS: " << e.what() << " during init save positions!";
        throw Exception(oss.str());
    }
}

std::string SavePositionServer::getSavedPathByFileName(std::string_view filename)
{
    std::string result(filename.substr(1).data());
    boost::replace_all(result, "/", "-");
    return result;
}

Position SavePositionServer::getCurrentSavedPositionFromFile(const std::string& filename)
{
    Position pos (0);

    try
    {
        std::ifstream ifs(filename.c_str(), std::ofstream::binary);
        ifs.read(reinterpret_cast<char*>(&pos), sizeof(pos));
    }
    catch(const std::ios::failure& e)
    {
        ERROR("Error read from FS: " << e.what());
    }
    return pos;
}

Position SavePositionServer::getSavedPosition(const std::string& filename)
{
    Position pos (0);

    SavedPointersTableInternal::const_accessor accessor;

    if(m_savedPointersTableInternal.find(accessor, getSavedPathByFileName(filename)))
    {
        pos = accessor->second;
    }

    return pos;
}

void SavePositionServer::savePosition(const std::string& filename, Position position)
{
    auto path = std::string(data_dir.data());
    path += '/' + getSavedPathByFileName(filename);

    try
    {
        std::ofstream ofs(path.c_str(), std::ofstream::binary);
        ofs.write(reinterpret_cast<char*>(&position), sizeof(position));
    }
    catch(const std::ios::failure& e)
    {
        ERROR("Error write saved state from FS: " << e.what());
    }
}

void SavePositionServer::erasePosition(const std::string& filename)
{
    auto path = std::string(data_dir.data());
    path += '/' + getSavedPathByFileName(filename);

    try
    {
        std::filesystem::remove(path);
    }
    catch(const std::ios::failure& e)
    {
        ERROR("Error erase saved state from FS: " << e.what());
    }
}