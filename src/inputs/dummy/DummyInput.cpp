//
// DummyInput.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "DummyInput.h"
#include "Dummy.h"

using namespace iqlogger;
using namespace iqlogger::inputs::dummy;

DummyInput::DummyInput(const config::SourceConfig& sourceConfig) : Input<Dummy>::Input(sourceConfig) {
  TRACE("DummyInput::DummyInput()");

  if (auto text = sourceConfig.getParam<std::string>("dummy_text"); text) {
    m_dummy_text = text.value();
  } else {
    auto dummy_length = sourceConfig.getParam<short>("dummy_length").value_or(1024);
    m_dummy_text = std::string("Dummy message: ") + std::string(dummy_length, 'A');
  }

  auto interval = sourceConfig.getParam<float>("interval").value_or(1);
  m_interval = std::chrono::milliseconds(static_cast<int>(interval * 1000));

  INFO("Initialized DUMMY Input with message: `" << m_dummy_text << "` "
                                                 << " Interval: " << m_interval.count() << " ms.");
}

DummyInput::~DummyInput() {
  TRACE("DummyInput::~DummyInput()");
}

void DummyInput::startImpl() {
  TRACE("DummyInput::start()");
  Input::startImpl();

  m_thread = std::thread([this]() {
    while (isRunning()) {
      if (!m_inputQueuePtr->enqueue(std::make_unique<Record<Dummy>>(std::string(m_dummy_text)))) {
        ERROR("Dummy Input queue is full... Dropping...");
      }

      std::this_thread::sleep_for(m_interval);
    }
  });
}

void DummyInput::stopImpl() {
  TRACE("DummyInput::stop()");
  Input::stopImpl();

  if (m_thread.joinable())
    m_thread.join();
}
