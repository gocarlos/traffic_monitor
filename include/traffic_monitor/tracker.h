// (c) 2017 Vigilatore

#ifndef INCLUDE_TRAFFIC_MONITOR_TRACKER_H_
#define INCLUDE_TRAFFIC_MONITOR_TRACKER_H_

#include <stdio.h>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "traffic_monitor/blob.h"
#include "traffic_monitor/drawer.h"
#include "traffic_monitor/settings.h"
#include "traffic_monitor/tools.h"

namespace traffic_monitor {

class Tracker {
  cv::VideoCapture vid_capture_;

  std::ofstream log_file_;
  // todo refactor this code
  cv::Mat imgFrame1;
  cv::Mat imgFrame2;
  cv::Mat imgFrame1L;
  cv::Mat imgFrame2L;

  std::vector<Blob> blobs_;

  cv::Point crossing_line_[2];

  int car_count_left{0};
  int car_count_right{0};
  bool first_frame = false;

  int frame_count_{0};

  int vertical_line_position_{0};

 public:
  Tracker();
  int run();
};

} /* namespace traffic_monitor */

#endif /* INCLUDE_TRAFFIC_MONITOR_TRACKER_H_ */
