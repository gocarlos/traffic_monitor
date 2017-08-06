// (c) 2017 Vigilatore

#include <traffic_monitor/settings.h>

namespace traffic_monitor {

Settings::Settings() {
  // TODO Auto-generated constructor stub
}

Settings::~Settings() {
  // TODO Auto-generated destructor stub
}

std::string Settings::path_to_log_file{"~/.config/traffic_monitor"};
bool Settings::with_gui_ = true;
bool Settings::debug_mode_ = false;

int Settings::FRAME_SCALE = 1;

} /* namespace traffic_monitor */
