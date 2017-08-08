// (c) 2017 Vigilatore

#include <traffic_monitor/tools.h>

namespace traffic_monitor {

float Tools::DistanceBetweenPoints(const cv::Point &point1,
                                   const cv::Point &point2) {
  return (std::sqrt(float(std::pow(std::abs(float(point1.x - point2.x)), 2) +
                          std::pow(std::abs(float(point1.y - point2.y)), 2))));
}

int Tools::CheckIfBlobsCrossedTheLine(std::vector<Blob> &blobs,
                                      int &vertical_line_position,
                                      int &car_count_left, int &number_cars,
                                      std::ofstream &log_file) {
  int at_least_1_blob_crossed_line = 0;

  for (auto blob : blobs) {
    if (blob.still_being_tracked_ == true &&
        blob.center_positions_.size() >= 2) {
      int prev_frame_index = (int)blob.center_positions_.size() - 2;
      int curr_frame_index = (int)blob.center_positions_.size() - 1;

      // Going left.
      if (blob.center_positions_[prev_frame_index].x > vertical_line_position &&
          blob.center_positions_[curr_frame_index].x <=
              vertical_line_position) {
        ++car_count_left;
        time_t now = time(0);
        char *dt = strtok(ctime(&now), "\n");

        LOG(INFO) << dt << ",1,0 (Left)";
        log_file << dt << ",1,0" << std::endl;
        at_least_1_blob_crossed_line = 1;
      }

      // Going right.
      if (blob.center_positions_[prev_frame_index].x < vertical_line_position &&
          blob.center_positions_[curr_frame_index].x >=
              vertical_line_position) {
        ++number_cars;
        time_t now = time(0);
        char *dt = strtok(ctime(&now), "\n");

        LOG(INFO) << dt << ",0,1 (Right)" << std::endl;
        log_file << dt << ",0,1" << std::endl;
        at_least_1_blob_crossed_line = 2;
      }
    }
  }

  return at_least_1_blob_crossed_line;
}

void Tools::MatchCurrentFrameBlobsToExistingBlobs(
    std::vector<Blob> &existing_blobs, std::vector<Blob> &current_frame_blobs) {
  for (auto &existing_blob : existing_blobs) {
    existing_blob.curr_match_found_or_newblob_ = false;

    existing_blob.PredictNextPosition();
  }

  for (auto &currentFrameBlob : current_frame_blobs) {
    int index_least_distance = 0;
    double least_distance = 100000.0;

    for (std::size_t i = 0; i < existing_blobs.size(); ++i) {
      if (existing_blobs[i].still_being_tracked_ == true) {
        double blob_distance = traffic_monitor::Tools::DistanceBetweenPoints(
            currentFrameBlob.center_positions_.back(),
            existing_blobs[i].predicted_next_position_);

        if (blob_distance < least_distance) {
          least_distance = blob_distance;
          index_least_distance = i;
        }
      }
    }

    if (least_distance < currentFrameBlob.curr_diagonal_size_ * 0.5) {
      AddBlobToExistingBlobs(currentFrameBlob, existing_blobs,
                             index_least_distance);
    } else {
      AddNewBlob(currentFrameBlob, existing_blobs);
    }
  }

  for (auto &existing_blob : existing_blobs) {
    if (existing_blob.curr_match_found_or_newblob_ == false) {
      ++existing_blob.num_consecutive_frames_without_match_;
    }

    if (existing_blob.num_consecutive_frames_without_match_ >= 5) {
      existing_blob.still_being_tracked_ = false;
    }
  }
}

void Tools::AddBlobToExistingBlobs(Blob &current_frame_blob,
                                   std::vector<Blob> &existing_blobs,
                                   int &index) {
  existing_blobs[index].curr_contour_ = current_frame_blob.curr_contour_;
  existing_blobs[index].curr_bounding_rect_ =
      current_frame_blob.curr_bounding_rect_;

  existing_blobs[index].center_positions_.push_back(
      current_frame_blob.center_positions_.back());

  existing_blobs[index].curr_diagonal_size_ =
      current_frame_blob.curr_diagonal_size_;
  existing_blobs[index].curr_aspect_ratio_ =
      current_frame_blob.curr_aspect_ratio_;

  existing_blobs[index].still_being_tracked_ = true;
  existing_blobs[index].curr_match_found_or_newblob_ = true;
}

void Tools::AddNewBlob(Blob &current_frame_blob,
                       std::vector<Blob> &existingBlobs) {
  current_frame_blob.curr_match_found_or_newblob_ = true;

  existingBlobs.push_back(current_frame_blob);
}

} /* namespace traffic_monitor */
