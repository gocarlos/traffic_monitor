/*
 * settings.h
 *
 *  Created on: Aug 6, 2017
 *      Author: gocarlos
 */

#ifndef INCLUDE_TRAFFIC_MONITOR_SETTINGS_H_
#define INCLUDE_TRAFFIC_MONITOR_SETTINGS_H_

#include <string>

#include <opencv2/core/core.hpp>

const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);

namespace traffic_monitor {

class Settings {
 public:
  Settings();
  virtual ~Settings();

  static std::string path_to_log_file_;
  static bool with_gui_;
  static bool debug_mode_;

  // Divide frame dimensions by this number.
  static int frame_scale_;
};

} /* namespace traffic_monitor */

#endif /* INCLUDE_TRAFFIC_MONITOR_SETTINGS_H_ */
