/*
 * settings.cc
 *
 *  Created on: Aug 6, 2017
 *      Author: gocarlos
 */

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

} /* namespace traffic_monitor */
