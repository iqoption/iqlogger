//
// InputConfig.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "InputConfig.h"

#include <filesystem>
#include <fstream>

#include "config/ConfigManager.h"
#ifdef IQLOGGER_WITH_PROCESSOR
#include <processor/ProcessorMachine.h>
#endif

using namespace iqlogger;
using namespace iqlogger::config;

template<>
bool SourceConfigCheckImpl<InputType::UNDEFINED>::operator()([[maybe_unused]] const SourceConfig& sourceConfig) const {
  ERROR("SourceConfig::check() failed when holds alternative in type of check!");
  return false;
}

void nlohmann::adl_serializer<InputConfig>::from_json(const json& j, iqlogger::config::InputConfig& inputConfig) {
  j.at("threads").get_to(inputConfig.threads);
  j.at("sources").get_to(inputConfig.sourceConfigs);
}

void nlohmann::adl_serializer<SourceConfig>::from_json(const json& j, iqlogger::config::SourceConfig& sourceConfig) {
  j.at("name").get_to(sourceConfig.name);
  j.at("type").get_to(sourceConfig.type);
  j.at("threads").get_to(sourceConfig.threads);

  try {
    j.at("max_queue_size").get_to(sourceConfig.max_queue_size);
    j.at("overflow_strategy").get_to(sourceConfig.overflow_strategy);
  } catch (const nlohmann::json::exception& e) {
    INFO("Overflow strategy for Input Queue " << sourceConfig.name << " not present. Using defaults...");
  }

#ifdef IQLOGGER_WITH_PROCESSOR
  if (auto it = j.find("processor"); it != j.end()) {
    try {
      sourceConfig.processor = it->get<ProcessorScript>();
    } catch (const json::exception& e) {
      sourceConfig.processor = std::nullopt;
    }
  }
#endif

  for (auto it = j.begin(); it != j.end(); ++it) {
    const auto& key = it.key();

    if (key != "name" && key != "type" && key != "threads" && key != "processor" && key != "max_queue_size" &&
        key != "overflow_strategy") {
      sourceConfig.params.emplace(key, it.value());
    }
  }

  sourceConfig.setChecker();
}

std::ostream& operator<<(std::ostream& os, InputType type) {
  return os << SourceConfig::input_type_to_str(type).data();
}

bool InputConfig::check() const {
  for (const auto& sourceConfig : sourceConfigs) {
    if (!sourceConfig.check())
      return false;
  }
  return true;
}

bool SourceConfig::check() const {
  bool result =
      std::visit([this](const auto& sourceConfigCheckImpl) -> bool { return sourceConfigCheckImpl(*this); }, m_checker);

#ifdef IQLOGGER_WITH_PROCESSOR
  return processor ? (result && processor.value().check()) : result;
#else
  return result;
#endif
}

#ifdef IQLOGGER_WITH_PROCESSOR
void nlohmann::adl_serializer<ProcessorScript>::from_json(const json& j,
                                                          iqlogger::config::ProcessorScript& processorScript) {
  auto script = j.get<std::string>();

  DEBUG("Processor source: " << script);

  auto fullPath = script,
       configPath = iqlogger::config::ConfigManager::getInstance()->getConfigPath().string() + '/' + script;

  TRACE("Probe: " << fullPath);
  if (std::filesystem::exists(fullPath)) {
    DEBUG("Find script file: " << fullPath);
    processorScript.loadFromFile(fullPath);
  } else {
    TRACE("Probe: " << configPath);
    if (std::filesystem::exists(configPath)) {
      DEBUG("Find script file: " << configPath);
      processorScript.loadFromFile(configPath);
    } else {
      processorScript.source = script;
    }
  }
}

bool ProcessorScript::loadFromFile(const std::string& filename) {
  std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
  std::ifstream::pos_type fileSize = ifs.tellg();

  if (fileSize < 0)
    return false;

  ifs.seekg(0, std::ios::beg);

  std::vector<char> bytes(fileSize);
  ifs.read(&bytes[0], fileSize);

  source = std::string(&bytes[0], fileSize);

  return !source.empty();
}

bool ProcessorScript::empty() const {
  return source.empty();
}

bool ProcessorScript::check() const {
  return processor::ProcessorMachine::getInstance()->checkScript(source);
}
#endif
