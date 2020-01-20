//
// GelfInput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//
#pragma once

#include "Server.h"
#include <inputs/IOInput.h>

namespace iqlogger::inputs::gelf::tcp {

class GelfInput : public IOInput<Gelf>
{
public:
  explicit GelfInput(const config::SourceConfig& sourceConfig);
  virtual ~GelfInput();

protected:
  void startImpl() override;

private:
  std::shared_ptr<Server> m_serverPtr;
  unsigned short m_port;
};

}  // namespace iqlogger::inputs::gelf::tcp
