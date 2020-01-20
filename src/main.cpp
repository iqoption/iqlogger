//
// IQLogger
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#include <csignal>
#include <thread>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "IQLogger.h"
#include "Version.h"

#include "config/ConfigManager.h"

int main(int argc, char* argv[]) {
  try {
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()("help", "This message")("version", "Get iqlogger version")(
        "check-config", "Check iqlogger config")("config", boost::program_options::value<std::string>(),
                                                 "Set config file");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 1;
    }

    if (vm.count("version")) {
      std::cout << "IQLogger v. " << IQLOGGER_VERSION << std::endl
                << "Copyright (C) 2018 IQOption Software, Inc." << std::endl;
      return 0;
    }

    if (!vm.count("config")) {
      std::cerr << "Usage: iqlogger --config=<path_to_config>" << std::endl;
      return 1;
    }

    auto configManagerPtr = iqlogger::config::ConfigManager::getInstance();

    bool checkConfig = vm.count("check-config");

    if (!configManagerPtr->load(vm["config"].as<std::string>(), checkConfig)) {
      std::cerr << "Loading configuration failed" << std::endl;
      return 1;
    }

    if (checkConfig) {
      std::cout << "Config is ok!" << std::endl;
      return 0;
    }

    auto iqloggerPtr = iqlogger::IQLogger::getInstance();
    iqloggerPtr->init({});
    iqloggerPtr->start();
  } catch (const boost::program_options::error& e) {
    CRITICAL("Params reading exception: " << e.what());
    return 1;
  } catch (const std::exception& e) {
    CRITICAL("Exception: " << e.what());
    return 1;
  } catch (...) {
    CRITICAL("Unhandled Exception!!!");
    return 1;
  }

  INFO("Final!");
  return 0;
}