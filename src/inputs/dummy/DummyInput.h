//
// DummyInput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <chrono>

#include "DummyInput.h"
#include "inputs/IOInput.h"

namespace iqlogger::inputs::dummy {

class DummyInput : public Input<Dummy>
{
public:
  explicit DummyInput(const config::SourceConfig& sourceConfig);
  virtual ~DummyInput();

protected:

  void startImpl() override;
  void stopImpl() override;

private:
  std::string m_dummy_text;
  std::chrono::milliseconds m_interval;
  std::thread m_thread;
};
}  // namespace iqlogger::inputs::dummy
