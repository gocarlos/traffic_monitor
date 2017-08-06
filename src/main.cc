// (c) 2017 Vigilatore

#include "cxxopts.hpp"
#include "traffic_monitor/settings.h"
#include "traffic_monitor/tracker.h"

using namespace traffic_monitor;

int main(int argc, char *argv[]) {
  //  google::InitGoogleLogging(argv[0]);

  cxxopts::Options options("Traffic Monitor", "TODO");

  // clang-format off
  options.add_options()
		  ("d,debug", "Enable debugging")
		  ("g,withgui", "Enable gui")
		  ("l,logfile", "Logging file name", cxxopts::value<std::string>()
		  );
  // clang-format on

  options.parse(argc, argv);
  Settings::with_gui_ = options["withgui"].as<bool>();
  LOG_IF(INFO, !Settings::with_gui_) << "Starting traffic monitor without GUI";

  std::string log_file = options["logfile"].as<std::string>();
  if (log_file.size() > 0) {
    Settings::path_to_log_file = log_file;
  }

  Tracker tracker;

  return tracker.run();
}
