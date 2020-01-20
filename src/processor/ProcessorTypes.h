//
// ProcessorTypes.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <memory>
#include <unordered_map>

#include "Message.h"

namespace iqlogger::processor {

using ProcessorScriptIndex = unsigned short;
using ScriptMap = std::unordered_map<ProcessorScriptIndex, std::string>;

using ProcessorRecord = std::pair<UniqueMessagePtr, ProcessorScriptIndex>;
using ProcessorRecordPtr = std::unique_ptr<ProcessorRecord>;

}  // namespace iqlogger::processor
