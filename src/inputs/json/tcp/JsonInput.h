//
// JsonInput.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//
#pragma once

#include "Server.h"
#include <inputs/IOInput.h>

namespace iqlogger::inputs::json::tcp {

class JsonInput : public IOInput<Json>
{
public:
  explicit JsonInput(const config::SourceConfig& sourceConfig);
  virtual ~JsonInput();

protected:
  void startImpl() override;

private:
  std::shared_ptr<Server> m_serverPtr;
  unsigned short m_port;
};

}  // namespace iqlogger::inputs::json::tcp
