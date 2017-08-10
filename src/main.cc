// (c) 2017 Vigilatore

#include <algorithm>
#include <csignal>
#include <fstream>
#include <vector>

#include "cxxopts.hpp"

#include "traffic_monitor/server.h"
#include "traffic_monitor/settings.h"
#include "traffic_monitor/tracker.h"

using namespace traffic_monitor;

Server server;

void inthand(int signum) {
  LOG(WARNING) << "Interrupt signal (" << signum << ") received.";
  Settings::stop_ = true;
  server.Close();
}

int main(int argc, char *argv[]) {
  //  google::InitGoogleLogging(argv[0]); does nothing, as we use miniglog
  signal(SIGINT, inthand);

  cxxopts::Options options("Traffic Monitor", "todo: longer app description");

  // clang-format off
	options.add_options()("d,debug", "Enable debugging")("g,withgui",
			"Enable gui")("l,logfile", "Logging file name",
			cxxopts::value<std::string>());
  // clang-format on

  options.parse(argc, argv);
  Settings::with_gui_ = options["withgui"].as<bool>();
  Settings::debug_mode_ = options["debug"].as<bool>();
  std::string log_file = options["logfile"].as<std::string>();

  if (log_file.size() > 0) {
    Settings::path_to_log_file_ = log_file;
  }

  LOG_IF(INFO, not Settings::with_gui_)
      << "Starting traffic monitor without GUI";
  LOG_IF(INFO, Settings::debug_mode_)
      << "Starting traffic monitor in debug mode";
  LOG(INFO) << "Path to logging file is: " << Settings::path_to_log_file_;

  // Start the car tracker.
  Tracker tracker(Tracker::camera, "0");
  std::thread tracker_thread(&Tracker::RunTracker, &tracker);

  // Start the web server.
  server.RunServer();

  // Close the web server.
  tracker_thread.join();

  LOG(WARNING) << "Closing traffic monitor.";
  return 0;
}
