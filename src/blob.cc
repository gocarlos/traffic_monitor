// (c) 2017 Vigilatore

#include <traffic_monitor/blob.h>

Blob::Blob(std::vector<cv::Point> _contour) {
  current_contour_ = _contour;

  current_bounding_rect_ = cv::boundingRect(current_contour_);

  cv::Point current_center;

  current_center.x = (current_bounding_rect_.x + current_bounding_rect_.x +
                      current_bounding_rect_.width) /
                     2;
  current_center.y = (current_bounding_rect_.y + current_bounding_rect_.y +
                      current_bounding_rect_.height) /
                     2;

  center_positions_.push_back(current_center);

  current_diagonal_size_ =
      std::sqrt((float)(std::pow(current_bounding_rect_.width, 2) +
                        std::pow(current_bounding_rect_.height, 2)));

  current_aspect_ratio_ =
      current_bounding_rect_.width / current_bounding_rect_.height;

  still_being_tracked_ = true;
  curr_match_found_or_newblob_ = true;

  num_consecutive_frames_without_match_ = 0;
}

void Blob::PredictNextPosition() {
  int num_positions = (int)center_positions_.size();

  if (num_positions == 1) {
    predicted_next_position_.x = center_positions_.back().x;
    predicted_next_position_.y = center_positions_.back().y;

  } else if (num_positions == 2) {
    int deltaX = center_positions_[1].x - center_positions_[0].x;
    int deltaY = center_positions_[1].y - center_positions_[0].y;

    predicted_next_position_.x = center_positions_.back().x + deltaX;
    predicted_next_position_.y = center_positions_.back().y + deltaY;

  } else if (num_positions == 3) {
    int sum_of_changes =
        ((center_positions_[2].x - center_positions_[1].x) * 2) +
        ((center_positions_[1].x - center_positions_[0].x) * 1);

    int deltaX = (int)std::round(sum_of_changes / 3.0f);

    int sumOfYChanges =
        ((center_positions_[2].y - center_positions_[1].y) * 2) +
        ((center_positions_[1].y - center_positions_[0].y) * 1);

    int deltaY = (int)std::round(sumOfYChanges / 3.0f);

    predicted_next_position_.x = center_positions_.back().x + deltaX;
    predicted_next_position_.y = center_positions_.back().y + deltaY;

  } else if (num_positions == 4) {
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

  } else if (num_positions >= 5) {
    int sum_of_x_changes = ((center_positions_[num_positions - 1].x -
                             center_positions_[num_positions - 2].x) *
                            4) +
                           ((center_positions_[num_positions - 2].x -
                             center_positions_[num_positions - 3].x) *
                            3) +
                           ((center_positions_[num_positions - 3].x -
                             center_positions_[num_positions - 4].x) *
                            2) +
                           ((center_positions_[num_positions - 4].x -
                             center_positions_[num_positions - 5].x) *
                            1);

    int deltaX = (int)std::round(sum_of_x_changes / 10.0f);

    int sum_of_y_changes = ((center_positions_[num_positions - 1].y -
                             center_positions_[num_positions - 2].y) *
                            4) +
                           ((center_positions_[num_positions - 2].y -
                             center_positions_[num_positions - 3].y) *
                            3) +
                           ((center_positions_[num_positions - 3].y -
                             center_positions_[num_positions - 4].y) *
                            2) +
                           ((center_positions_[num_positions - 4].y -
                             center_positions_[num_positions - 5].y) *
                            1);

    int deltaY = (int)std::round(sum_of_y_changes / 10.0f);

    predicted_next_position_.x = center_positions_.back().x + deltaX;
    predicted_next_position_.y = center_positions_.back().y + deltaY;

  } else {
    LOG(FATAL) << "You should not have arrived here!";
  }
}
