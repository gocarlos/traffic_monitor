// (c) 2017 Vigilatore

#ifndef INCLUDE_TRAFFIC_MONITOR_DRAWER_BLOB_H_
#define INCLUDE_TRAFFIC_MONITOR_DRAWER_BLOB_H_

#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <glog/logging.h>

class Blob {
 public:
  std::vector<cv::Point> current_contour_;

  cv::Rect current_bounding_rect_;

  std::vector<cv::Point> center_positions_;

  double current_diagonal_size_;
  double current_aspect_ratio_;

  bool current_match_found_or_newblob_;

  bool still_being_tracked_;

  int intNumOfConsecutiveFramesWithoutAMatch;

  cv::Point predicted_next_position_;

  Blob(std::vector<cv::Point> _contour);
  void PredictNextPosition();
};

#endif  // INCLUDE_TRAFFIC_MONITOR_DRAWER_BLOB_H_
