/*
 * settings.h
 *
 *  Created on: Aug 6, 2017
 *      Author: gocarlos
 */

#ifndef INCLUDE_TRAFFIC_MONITOR_SETTINGS_H_
#define INCLUDE_TRAFFIC_MONITOR_SETTINGS_H_

#include <string>

namespace traffic_monitor {

class Settings {
 public:
  Settings();
  virtual ~Settings();
  static std::string path_to_log_file;
  static bool with_gui_;
  static bool debug_mode_;
};

} /* namespace traffic_monitor */

#endif /* INCLUDE_TRAFFIC_MONITOR_SETTINGS_H_ */
