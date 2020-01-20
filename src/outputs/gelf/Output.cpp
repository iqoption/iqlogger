//
// Output.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include "Gelf.h"
#include "Message.h"
#include "outputs/OutputInterface.h"
#include "outputs/Record.h"
#include "tcp/GelfOutput.h"

#ifdef IQLOGGER_INPUT_MODULE_JOURNAL
#include "inputs/journal/Journal.h"
using namespace iqlogger::formats::journal;
#endif

#ifdef IQLOGGER_INPUT_MODULE_TAIL
#include "inputs/tail/Tail.h"
using namespace iqlogger::formats::tail;
#endif

#ifdef IQLOGGER_INPUT_MODULE_GELF
#include "inputs/gelf/Gelf.h"
using namespace iqlogger::formats::gelf;
#endif

#ifdef IQLOGGER_INPUT_MODULE_JSON
#include "inputs/json/Json.h"
using namespace iqlogger::formats::json;
#endif

#ifdef IQLOGGER_INPUT_MODULE_DUMMY
#include "inputs/dummy/Dummy.h"
#endif

#ifdef IQLOGGER_WITH_PROCESSOR
#include "processor/ProcessorMessage.h"
using namespace iqlogger::processor;
#endif

using namespace iqlogger;
using namespace iqlogger::outputs;
using namespace iqlogger::outputs::gelf;
using namespace iqlogger::formats::gelf;

template<>
OutputPtr OutputInterface::instantiateOutput<config::OutputType::GELF>(
    const config::DestinationConfig& destinationConfig,
    iqlogger::metrics::atomic_metric_t& total_outputs_send_counter) {
  if (destinationConfig.getParam<config::Protocol>("protocol") == config::Protocol::TCP) {
    return std::make_unique<outputs::gelf::tcp::GelfOutput>(destinationConfig, total_outputs_send_counter);
  } else {
    std::ostringstream oss;
    oss << "Protocol for input " << destinationConfig.name << " is not supported!";
    throw Exception(oss.str());
  }
}

#ifdef IQLOGGER_INPUT_MODULE_JOURNAL
// Journal
template<>
template<>
Record<Gelf>::Record(const iqlogger::inputs::journal::Journal::MessageT& message) {
  TRACE("outputs::Record::Record() [<Gelf>] (Journal)");
  DEBUG("Transform Journal -> Gelf: ");

  // @TODO
  const auto& data = message.getData();

  Gelf::MessageT gelf;

  try {
    std::visit([&gelf](const auto& value) { gelf.setField("short_message", value); TRACE("MESSAGE=" << value);  }, data.at("MESSAGE"));
  } catch (const std::out_of_range& e) {
    gelf.setField("short_message", message.exportMessage());
  }

  gelf.setField("timestamp", message.getTimestamp() / (double)1'000'000);

  try {
    std::visit([&gelf](const auto& value) { gelf.setField("host", value); }, data.at("_HOSTNAME"));
  } catch (const std::out_of_range& e) {
  }

  for (const auto& [key, value] : data) {
    std::visit(
        [&gelf, &key = std::as_const(key)](const auto& v) {
          if (key != "MESSAGE" && key != "_HOSTNAME")
            gelf.setField(key, v);
        },
        value);
  }

  m_data = gelf.exportMessage();
}
#endif

#ifdef IQLOGGER_INPUT_MODULE_TAIL
// Tail
template<>
template<>
Record<Gelf>::Record(const iqlogger::inputs::tail::Tail::MessageT& message) {
  TRACE("outputs::Record::Record() [<Gelf>] (Tail)");

  Gelf::MessageT gelf;

  auto messageStr = message.exportMessage();

  DEBUG("Transform Tail -> Gelf: " << messageStr);

  try {
    auto json = nlohmann::json::parse(messageStr);

    if (!json.count("data")) {
      // Пытаемся собрать GELF из JSON как есть
      Gelf::MessageT g(messageStr);
      g.setField("file", message.getFilename());
      m_data = g.exportMessage();
      return;
    } else if (auto d = json.at("data"); d.is_object()) {
      for (nlohmann::json::iterator it = d.begin(); it != d.end(); ++it) {
        // @TODO Перенести логику в setField()
        if (it.value().is_string() || it.value().is_number() || it.value().is_boolean()) {
          gelf.setField(it.key(), it.value());
        }
      }
    } else {
      std::ostringstream oss;
      oss << "Error decode tail message from " << messageStr;
      throw Exception(oss.str());
    }
  } catch (const nlohmann::json::parse_error& e) {
    gelf.setField("short_message", messageStr);
  } catch (const nlohmann::json::exception& e) {
    gelf.setField("short_message", messageStr);
    DEBUG("Warning: " << e.what() << " due parsing data: " << messageStr);
  }

  gelf.setField("file", message.getFilename());

  m_data = gelf.exportMessage();
}
#endif

#ifdef IQLOGGER_INPUT_MODULE_GELF
// Gelf
template<>
template<>
Record<Gelf>::Record(const iqlogger::inputs::gelf::Gelf::MessageT& message) : m_data(message.exportMessage()) {
  TRACE("outputs::Record::Record() [<Gelf>] (Gelf)");
  DEBUG("Transform Gelf -> Gelf: ");
}
#endif

#ifdef IQLOGGER_INPUT_MODULE_JSON
// Json
template<>
template<>
Record<Gelf>::Record(const iqlogger::inputs::json::Json::MessageT& message) {
  TRACE("outputs::Record::Record() [<Gelf>] (Json)");
  DEBUG("Transform Json -> Gelf: ");

  Gelf::MessageT gelf;

  for (auto it = message.jbegin(); it != message.jend(); ++it) {
    // @TODO Перенести логику в setField()
    if (it.value().is_string() || it.value().is_number() || it.value().is_boolean()) {
      gelf.setField(it.key(), it.value());
    }
  }

  m_data = gelf.exportMessage();
}
#endif

#ifdef IQLOGGER_INPUT_MODULE_DUMMY
// Dummy
template<>
template<>
Record<Gelf>::Record(const iqlogger::inputs::dummy::Dummy::MessageT& message) {
  TRACE("outputs::Record::Record() [<Gelf>] (Dummy)");
  DEBUG("Transform Dummy -> Gelf: ");
  Gelf::MessageT gelf;
  gelf.setField("short_message", message);
  m_data = gelf.exportMessage();
}
#endif

#ifdef IQLOGGER_WITH_PROCESSOR
// Processor
template<>
template<>
Record<Gelf>::Record(const ProcessorMessage& message) : m_data(message.exportMessage()) {
  TRACE("outputs::Record::Record() [<Gelf>] (Processor)");
  DEBUG("Transform Processor -> Gelf: ");
}
#endif
