/*
 * drawer.h
 *
 *  Created on: Aug 5, 2017
 *      Author: gocarlos
 */

#ifndef INCLUDE_TRAFFIC_MONITOR_DRAWER_H_
#define INCLUDE_TRAFFIC_MONITOR_DRAWER_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "traffic_monitor/blob.h"
#include "traffic_monitor/tools.h"

namespace traffic_monitor {

class Drawer {
 public:
  Drawer();
  virtual ~Drawer();

  static void drawBlobInfoOnImage(std::vector<Blob> &blobs,
                                  cv::Mat &imgFrame2Copy);

  static void drawCarCountOnImage(int &carCountL, int &carCountR,
                                  cv::Mat &imgFrame2Copy);

  static void drawAndShowContours(cv::Size imageSize,
                                  std::vector<std::vector<cv::Point>> contours,
                                  std::string strImageName);

  static void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs,
                                  std::string strImageName);
};

} /* namespace traffic_monitor */

#endif /* INCLUDE_TRAFFIC_MONITOR_DRAWER_H_ */
