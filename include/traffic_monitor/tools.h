/*
 * Tools.h
 *
 *  Created on: Aug 5, 2017
 *      Author: gocarlos
 */

#ifndef INCLUDE_TRAFFIC_MONITOR_TOOLS_H_
#define INCLUDE_TRAFFIC_MONITOR_TOOLS_H_

#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "traffic_monitor/blob.h"

#define FRAME_SCALE 1  // divide frame dimensions by this number

const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);

namespace traffic_monitor {

class Tools {
 public:
  Tools();
  virtual ~Tools();
  static double distanceBetweenPoints(cv::Point point1, cv::Point point2);

  static bool checkIfBlobsCrossedTheLine(std::vector<Blob> &blobs,
                                         int &intVerticalLinePosition,
                                         int &carCountL, int &carCountR,
                                         std::ofstream &myfile);
  static void matchCurrentFrameBlobsToExistingBlobs(
      std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
  static void addBlobToExistingBlobs(Blob &currentFrameBlob,
                              std::vector<Blob> &existingBlobs, int &intIndex);
  static void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs);
};

} /* namespace traffic_monitor */

#endif /* INCLUDE_TRAFFIC_MONITOR_TOOLS_H_ */
