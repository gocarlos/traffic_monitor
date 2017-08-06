// (c) 2017 Vigilatore

#include <traffic_monitor/blob.h>

Blob::Blob(std::vector<cv::Point> _contour) {
  current_contour_ = _contour;

  current_bounding_rect_ = cv::boundingRect(current_contour_);

  cv::Point currentCenter;

  currentCenter.x = (current_bounding_rect_.x + current_bounding_rect_.x +
                     current_bounding_rect_.width) /
                    2;
  currentCenter.y = (current_bounding_rect_.y + current_bounding_rect_.y +
                     current_bounding_rect_.height) /
                    2;

  center_positions_.push_back(currentCenter);

  dblCurrentDiagonalSize =
      std::sqrt((float)(std::pow(current_bounding_rect_.width, 2) +
                        std::pow(current_bounding_rect_.height, 2)));

  dblCurrentAspectRatio = (float)current_bounding_rect_.width /
                          (float)current_bounding_rect_.height;

  still_being_tracked_ = true;
  current_match_found_or_newblob_ = true;

  intNumOfConsecutiveFramesWithoutAMatch = 0;
}

void Blob::PredictNextPosition() {
  int numPositions = (int)center_positions_.size();

  if (numPositions == 1) {
    predicted_next_position_.x = center_positions_.back().x;
    predicted_next_position_.y = center_positions_.back().y;

  } else if (numPositions == 2) {
    int deltaX = center_positions_[1].x - center_positions_[0].x;
    int deltaY = center_positions_[1].y - center_positions_[0].y;

    predicted_next_position_.x = center_positions_.back().x + deltaX;
    predicted_next_position_.y = center_positions_.back().y + deltaY;

  } else if (numPositions == 3) {
    int sumOfXChanges =
        ((center_positions_[2].x - center_positions_[1].x) * 2) +
        ((center_positions_[1].x - center_positions_[0].x) * 1);

    int deltaX = (int)std::round(sumOfXChanges / 3.0f);

    int sumOfYChanges =
        ((center_positions_[2].y - center_positions_[1].y) * 2) +
        ((center_positions_[1].y - center_positions_[0].y) * 1);

    int deltaY = (int)std::round(sumOfYChanges / 3.0f);

    predicted_next_position_.x = center_positions_.back().x + deltaX;
    predicted_next_position_.y = center_positions_.back().y + deltaY;

  } else if (numPositions == 4) {
    int sumOfXChanges =
        ((center_positions_[3].x - center_positions_[2].x) * 3) +
        ((center_positions_[2].x - center_positions_[1].x) * 2) +
        ((center_positions_[1].x - center_positions_[0].x) * 1);

    int deltaX = (int)std::round(sumOfXChanges / 6.0f);

    int sumOfYChanges =
        ((center_positions_[3].y - center_positions_[2].y) * 3) +
        ((center_positions_[2].y - center_positions_[1].y) * 2) +
        ((center_positions_[1].y - center_positions_[0].y) * 1);

    int deltaY = (int)std::round(sumOfYChanges / 6.0f);

    predicted_next_position_.x = center_positions_.back().x + deltaX;
    predicted_next_position_.y = center_positions_.back().y + deltaY;

  } else if (numPositions >= 5) {
    int sumOfXChanges = ((center_positions_[numPositions - 1].x -
                          center_positions_[numPositions - 2].x) *
                         4) +
                        ((center_positions_[numPositions - 2].x -
                          center_positions_[numPositions - 3].x) *
                         3) +
                        ((center_positions_[numPositions - 3].x -
                          center_positions_[numPositions - 4].x) *
                         2) +
                        ((center_positions_[numPositions - 4].x -
                          center_positions_[numPositions - 5].x) *
                         1);

    int deltaX = (int)std::round(sumOfXChanges / 10.0f);

    int sumOfYChanges = ((center_positions_[numPositions - 1].y -
                          center_positions_[numPositions - 2].y) *
                         4) +
                        ((center_positions_[numPositions - 2].y -
                          center_positions_[numPositions - 3].y) *
                         3) +
                        ((center_positions_[numPositions - 3].y -
                          center_positions_[numPositions - 4].y) *
                         2) +
                        ((center_positions_[numPositions - 4].y -
                          center_positions_[numPositions - 5].y) *
                         1);

    int deltaY = (int)std::round(sumOfYChanges / 10.0f);

    predicted_next_position_.x = center_positions_.back().x + deltaX;
    predicted_next_position_.y = center_positions_.back().y + deltaY;

  } else {
    // should never get here
  }
}
