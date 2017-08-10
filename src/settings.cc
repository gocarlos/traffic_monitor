// (c) 2017 Vigilatore

#include <traffic_monitor/settings.h>

namespace traffic_monitor {

std::string Settings::path_to_log_file_{"~/.config/traffic_monitor"};

// Shows the GUI if set to true.
bool Settings::with_gui_ = false;

// Print more debug messages if set to true.
bool Settings::debug_mode_ = false;

int Settings::frame_scale_ = 1;

int Settings::ws_server_port_ = 8090;
int Settings::http_server_port_ = 8080;

bool Settings::stop_ = false;

}  // namespace traffic_monitor
