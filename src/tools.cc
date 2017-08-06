// (c) 2017 Vigilatore

#include <traffic_monitor/tools.h>

namespace traffic_monitor {

Tools::Tools() {}

float Tools::DistanceBetweenPoints(const cv::Point &point1,
                                   const cv::Point &point2) {
  return (std::sqrt(float(std::pow(std::abs(float(point1.x - point2.x)), 2) +
                          std::pow(std::abs(float(point1.y - point2.y)), 2))));
}

int Tools::CheckIfBlobsCrossedTheLine(std::vector<Blob> &blobs,
                                      int &intVerticalLinePosition,
                                      int &carCountL, int &number_cars,
                                      std::ofstream &myfile) {
  int blnAtLeastOneBlobCrossedTheLine = 0;

  for (auto blob : blobs) {
    if (blob.still_being_tracked_ == true &&
        blob.center_positions_.size() >= 2) {
      int prevFrameIndex = (int)blob.center_positions_.size() - 2;
      int currFrameIndex = (int)blob.center_positions_.size() - 1;

      // Going left.
      if (blob.center_positions_[prevFrameIndex].x > intVerticalLinePosition &&
          blob.center_positions_[currFrameIndex].x <= intVerticalLinePosition) {
        carCountL++;
        time_t now = time(0);
        char *dt = strtok(ctime(&now), "\n");
        ;
        LOG(INFO) << dt << ",1,0 (Left)";
        myfile << dt << ",1,0" << std::endl;
        blnAtLeastOneBlobCrossedTheLine = 1;
      }

      // Going right.
      if (blob.center_positions_[prevFrameIndex].x < intVerticalLinePosition &&
          blob.center_positions_[currFrameIndex].x >= intVerticalLinePosition) {
        ++number_cars;
        time_t now = time(0);
        char *dt = strtok(ctime(&now), "\n");
        ;
        LOG(INFO) << dt << ",0,1 (Right)" << std::endl;
        myfile << dt << ",0,1" << std::endl;
        blnAtLeastOneBlobCrossedTheLine = 2;
      }
    }
  }

  return blnAtLeastOneBlobCrossedTheLine;
}

void Tools::MatchCurrentFrameBlobsToExistingBlobs(
    std::vector<Blob> &existing_blobs, std::vector<Blob> &current_frame_blobs) {
  for (auto &existing_blob : existing_blobs) {
    existing_blob.current_match_found_or_newblob_ = false;

    existing_blob.PredictNextPosition();
  }

  for (auto &currentFrameBlob : current_frame_blobs) {
    int index_least_distance = 0;
    double least_distance = 100000.0;

    for (std::size_t i = 0; i < existing_blobs.size(); ++i) {
      if (existing_blobs[i].still_being_tracked_ == true) {
        double dblDistance = traffic_monitor::Tools::DistanceBetweenPoints(
            currentFrameBlob.center_positions_.back(),
            existing_blobs[i].predicted_next_position_);

        if (dblDistance < least_distance) {
          least_distance = dblDistance;
          index_least_distance = i;
        }
      }
    }

    if (least_distance < currentFrameBlob.current_diagonal_size_ * 0.5) {
      AddBlobToExistingBlobs(currentFrameBlob, existing_blobs,
                             index_least_distance);
    } else {
      AddNewBlob(currentFrameBlob, existing_blobs);
    }
  }

  for (auto &existingBlob : existing_blobs) {
    if (existingBlob.current_match_found_or_newblob_ == false) {
      existingBlob.num_consecutive_frames_without_match_++;
    }

    if (existingBlob.num_consecutive_frames_without_match_ >= 5) {
      existingBlob.still_being_tracked_ = false;
    }
  }
}

void Tools::AddBlobToExistingBlobs(Blob &currentFrameBlob,
                                   std::vector<Blob> &existing_blobs,
                                   int &index) {
  existing_blobs[index].current_contour_ = currentFrameBlob.current_contour_;
  existing_blobs[index].current_bounding_rect_ =
      currentFrameBlob.current_bounding_rect_;

  existing_blobs[index].center_positions_.push_back(
      currentFrameBlob.center_positions_.back());

  existing_blobs[index].current_diagonal_size_ =
      currentFrameBlob.current_diagonal_size_;
  existing_blobs[index].current_aspect_ratio_ =
      currentFrameBlob.current_aspect_ratio_;

  existing_blobs[index].still_being_tracked_ = true;
  existing_blobs[index].current_match_found_or_newblob_ = true;
}

void Tools::AddNewBlob(Blob &currentFrameBlob,
                       std::vector<Blob> &existingBlobs) {
  currentFrameBlob.current_match_found_or_newblob_ = true;

  existingBlobs.push_back(currentFrameBlob);
}

} /* namespace traffic_monitor */
