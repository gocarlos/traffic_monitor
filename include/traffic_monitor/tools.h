// (c) 2017 Vigilatore

#ifndef INCLUDE_TRAFFIC_MONITOR_TOOLS_H_
#define INCLUDE_TRAFFIC_MONITOR_TOOLS_H_

#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "traffic_monitor/blob.h"

namespace traffic_monitor {

class Tools {
 public:
  Tools();
  virtual ~Tools();

  static double DistanceBetweenPoints(const cv::Point &point1,
                                      const cv::Point &point2);

  static int CheckIfBlobsCrossedTheLine(std::vector<Blob> &blobs,
                                        int &intVerticalLinePosition,
                                        int &carCountL, int &carCountR,
                                        std::ofstream &myfile);
  static void MatchCurrentFrameBlobsToExistingBlobs(
      std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
  static void AddBlobToExistingBlobs(Blob &currentFrameBlob,
                                     std::vector<Blob> &existingBlobs,
                                     int &intIndex);
  static void AddNewBlob(Blob &currentFrameBlob,
                         std::vector<Blob> &existingBlobs);
};

} /* namespace traffic_monitor */

#endif /* INCLUDE_TRAFFIC_MONITOR_TOOLS_H_ */
