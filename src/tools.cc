// (c) 2017 Vigilatore

#include <traffic_monitor/tools.h>

namespace traffic_monitor {

Tools::Tools() {
  // TODO Auto-generated constructor stub
}

Tools::~Tools() {
  // TODO Auto-generated destructor stub
}

double Tools::distanceBetweenPoints(cv::Point point1, cv::Point point2) {
  int intX = std::abs(point1.x - point2.x);
  int intY = std::abs(point1.y - point2.y);

  return (std::sqrt(std::pow(intX, 2) + std::pow(intY, 2)));
}

bool Tools::checkIfBlobsCrossedTheLine(std::vector<Blob> &blobs,
                                       int &intVerticalLinePosition,
                                       int &carCountL, int &carCountR,
                                       std::ofstream &myfile) {
  bool blnAtLeastOneBlobCrossedTheLine = 0;

  for (auto blob : blobs) {
    if (blob.blnStillBeingTracked == true &&
        blob.center_positions_.size() >= 2) {
      int prevFrameIndex = (int)blob.center_positions_.size() - 2;
      int currFrameIndex = (int)blob.center_positions_.size() - 1;

      // going left
      if (blob.center_positions_[prevFrameIndex].x > intVerticalLinePosition &&
          blob.center_positions_[currFrameIndex].x <= intVerticalLinePosition) {
        carCountL++;
        time_t now = time(0);
        char *dt = strtok(ctime(&now), "\n");
        ;
        std::cout << dt << ",1,0 (Left)" << std::endl;
        myfile << dt << ",1,0" << std::endl;
        blnAtLeastOneBlobCrossedTheLine = 1;
      }

      // going right
      if (blob.center_positions_[prevFrameIndex].x < intVerticalLinePosition &&
          blob.center_positions_[currFrameIndex].x >= intVerticalLinePosition) {
        carCountR++;
        time_t now = time(0);
        char *dt = strtok(ctime(&now), "\n");
        ;
        std::cout << dt << ",0,1 (Right)" << std::endl;
        myfile << dt << ",0,1" << std::endl;
        blnAtLeastOneBlobCrossedTheLine = 2;
      }
    }
  }

  return blnAtLeastOneBlobCrossedTheLine;
}

void Tools::matchCurrentFrameBlobsToExistingBlobs(
    std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs) {
  for (auto &existingBlob : existingBlobs) {
    existingBlob.blnCurrentMatchFoundOrNewBlob = false;

    existingBlob.PredictNextPosition();
  }

  for (auto &currentFrameBlob : currentFrameBlobs) {
    int intIndexOfLeastDistance = 0;
    double dblLeastDistance = 100000.0;

    for (unsigned int i = 0; i < existingBlobs.size(); i++) {
      if (existingBlobs[i].blnStillBeingTracked == true) {
        double dblDistance = traffic_monitor::Tools::distanceBetweenPoints(
            currentFrameBlob.center_positions_.back(),
            existingBlobs[i].predicted_next_position_);

        if (dblDistance < dblLeastDistance) {
          dblLeastDistance = dblDistance;
          intIndexOfLeastDistance = i;
        }
      }
    }

    if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5) {
      addBlobToExistingBlobs(currentFrameBlob, existingBlobs,
                             intIndexOfLeastDistance);
    } else {
      addNewBlob(currentFrameBlob, existingBlobs);
    }
  }

  for (auto &existingBlob : existingBlobs) {
    if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
      existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
    }

    if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
      existingBlob.blnStillBeingTracked = false;
    }
  }
}

void Tools::addBlobToExistingBlobs(Blob &currentFrameBlob,
                                   std::vector<Blob> &existingBlobs,
                                   int &intIndex) {
  existingBlobs[intIndex].current_contour_ = currentFrameBlob.current_contour_;
  existingBlobs[intIndex].current_bounding_rect_ =
      currentFrameBlob.current_bounding_rect_;

  existingBlobs[intIndex].center_positions_.push_back(
      currentFrameBlob.center_positions_.back());

  existingBlobs[intIndex].dblCurrentDiagonalSize =
      currentFrameBlob.dblCurrentDiagonalSize;
  existingBlobs[intIndex].dblCurrentAspectRatio =
      currentFrameBlob.dblCurrentAspectRatio;

  existingBlobs[intIndex].blnStillBeingTracked = true;
  existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}

void Tools::addNewBlob(Blob &currentFrameBlob,
                       std::vector<Blob> &existingBlobs) {
  currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;

  existingBlobs.push_back(currentFrameBlob);
}

} /* namespace traffic_monitor */
