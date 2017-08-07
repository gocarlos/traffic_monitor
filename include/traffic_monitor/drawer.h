// (c) 2017 Vigilatore

#ifndef INCLUDE_TRAFFIC_MONITOR_DRAWER_H_
#define INCLUDE_TRAFFIC_MONITOR_DRAWER_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "traffic_monitor/blob.h"
#include "traffic_monitor/settings.h"
#include "traffic_monitor/tools.h"

namespace traffic_monitor {

class Drawer {
 public:
 // todo make names shorter
  static void DrawBlobInfoOnImage(std::vector<Blob> &blobs,
                                  cv::Mat &imgFrame2Copy);

  static void DrawCarCountOnImage(int &carCountL, int &carCountR,
                                  cv::Mat &imgFrame2Copy);

  static void DrawAndShowContours(cv::Size imageSize,
                                  std::vector<std::vector<cv::Point>> contours,
                                  std::string strImageName);

  static void DrawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs,
                                  std::string strImageName);
};

} /* namespace traffic_monitor */

#endif /* INCLUDE_TRAFFIC_MONITOR_DRAWER_H_ */
