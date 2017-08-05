/*
 * tracker.h
 *
 *  Created on: Aug 5, 2017
 *      Author: gocarlos
 */

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
#include "traffic_monitor/tools.h"
namespace traffic_monitor {

class Tracker {
  cv::VideoCapture capVideo;

  std::ofstream myfile;

  cv::Mat imgFrame1;
  cv::Mat imgFrame2;
  cv::Mat imgFrame1L;
  cv::Mat imgFrame2L;

  std::vector<Blob> blobs;

  cv::Point crossingLine[2];

  int carCountL{0};
  int carCountR{0};
  bool blnFirstFrame = false;
  char chCheckForEscKey{0};

  int frameCount{0};

  int intVerticalLinePosition{0};

 public:
  Tracker();
  virtual ~Tracker();
  int run();
};

} /* namespace traffic_monitor */

#endif /* INCLUDE_TRAFFIC_MONITOR_TRACKER_H_ */
