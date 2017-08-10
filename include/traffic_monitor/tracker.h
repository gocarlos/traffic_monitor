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

  cv::Mat img_frame_1_;
  cv::Mat img_frame_2_;
  cv::Mat img_frame_1_large_;
  cv::Mat img_frame_2_large_;

  std::vector<Blob> blobs_;

  cv::Point crossing_line_[2];

  int car_count_left_{0};
  int car_count_right_{0};
  bool first_frame_ = false;

  int frame_count_{0};

  int vertical_line_position_{0};

 public:
  enum Input { camera, file } input_;
  int camera_number_;
  std::string video_path_;

  Tracker(Tracker::Input input, std::string stream);
  void RunTracker();
};

}  // namespace traffic_monitor

#endif  // INCLUDE_TRAFFIC_MONITOR_TRACKER_H_
