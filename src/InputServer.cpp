//
// InputServer.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include <boost/asio.hpp>
#include <thread>

#include "Exception.h"
#include "InputServer.h"
#include "config/ConfigManager.h"

using namespace iqlogger;

InputServer::InputServer() :
    TaskInterface::TaskInterface(),
    m_thread_num{config::ConfigManager::getInstance()->getConfig()->inputConfig.threads},
    m_receive_counter(0) {
  INFO("Configure input server with " << m_thread_num << " threads...");

  for (const auto& sourceConfig : config::ConfigManager::getInstance()->getConfig()->inputConfig.sourceConfigs) {
    INFO("Creating input source " << sourceConfig.type << " " << sourceConfig.name);
    m_inputs.emplace_back(inputs::InputInterface::instantiate(sourceConfig));
  }
}

void InputServer::initImpl(std::any) {
  INFO("Configuring input server...");
  for (const auto& inputPtr : m_inputs) {
    inputPtr->init({});
  }
}

void InputServer::startImpl() {
  // 1. Start servers
  INFO("Start input server...");

  INFO("Start modules for input server...");
  for (const auto& inputPtr : m_inputs) {
    inputPtr->startModule();
  }

  INFO("Start inputs...");
  for (const auto& inputPtr : m_inputs) {
    inputPtr->start();
  }

  INFO("Start inport threads...");
  for (size_t i = 0; i < m_thread_num; ++i) {
    m_threads.emplace_back(std::thread([this]() {
      unsigned long counter;

      while (isRunning()) {
        counter = 0;

        for (const auto& inputPtr : m_inputs) {
          counter += inputPtr->importMessages();
        }

        m_receive_counter += counter;

        if (!counter)
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }));
  }

  INFO("Register metrics for InputServer");

  metrics::MetricsController::getInstance()->registerMetric("inputs.receive_total",
                                                            [this]() { return m_receive_counter.load(); });

  INFO("Inputs started. Waiting for logs...");
}

void InputServer::stopImpl() {
  TRACE("InputServer::stop() ->");

  for (const auto& inputPtr : m_inputs) {
    inputPtr->stop();
  }

  INFO("Stop modules for input server...");
  for (const auto& inputPtr : m_inputs) {
    inputPtr->stopModule();
  }

  TRACE("Join input io threads... ");
  for (auto& t : m_threads) {
    if (t.joinable())
      t.join();
  }

  TRACE("InputServer::stop() <-");
}

InputServer::~InputServer() {
  TRACE("InputServer::~InputServer()");
}
