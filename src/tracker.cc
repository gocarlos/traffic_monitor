// (c) 2017 Vigilatore

#include <traffic_monitor/tracker.h>

namespace traffic_monitor {

Tracker::Tracker() {
  LOG(INFO) << "Setting up the tracker.";

  // capVideo.open("../movie.mp4");
  std::size_t num_cameras{0};
  for (std::size_t i = 0; i < 5; ++i) {
    capVideo.open(i);
    if (!capVideo.isOpened()) {
      ++num_cameras;
    }
  }
  if (num_cameras > 0) {
    LOG(INFO) << "There are " << num_cameras
              << " available. \nTaking the first one";
  }
  // TODO(gocarlos) choose the camera or video here.
  capVideo.open(0);

  myfile.open("../logging/OpenCV-" + std::to_string(time(0)) + ".txt");
  std::cout << "Logging to: \"/tmp/OpenCV-"
            << "-" << std::to_string(time(0)) << ".txt\"" << std::endl;

  myfile << "\"Timestamp\",\"Left\",\"Right\"" << std::endl;

  capVideo.read(imgFrame1L);
  capVideo.read(imgFrame2L);

  resize(imgFrame1L, imgFrame1,
         cv::Size(imgFrame1L.size().width / FRAME_SCALE,
                  imgFrame1L.size().height / FRAME_SCALE));
  resize(imgFrame2L, imgFrame2,
         cv::Size(imgFrame2L.size().width / FRAME_SCALE,
                  imgFrame2L.size().height / FRAME_SCALE));

  intVerticalLinePosition = std::round(imgFrame1.cols * 0.50);

  crossingLine[0].y = 0;
  crossingLine[0].x = intVerticalLinePosition;

  crossingLine[1].y = imgFrame1.rows - 1;
  crossingLine[1].x = intVerticalLinePosition;

  frameCount = 2;
  bool blnFirstFrame = true;
}

Tracker::~Tracker() {
  // TODO Auto-generated destructor stub
}

int Tracker::run() {
  LOG(INFO) << "Entering the main loop.";

  while (capVideo.isOpened() && chCheckForEscKey != 27) {
    std::vector<Blob> currentFrameBlobs;

    cv::Mat imgFrame1Copy = imgFrame1.clone();
    cv::Mat imgFrame2Copy = imgFrame2.clone();

    cv::Mat imgDifference;
    cv::Mat imgThresh;

    cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
    cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

    cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
    cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);

    cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

    cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);

    // cv::imshow("imgThresh", imgThresh);

    cv::Mat structuringElement3x3 =
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat structuringElement5x5 =
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat structuringElement7x7 =
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
    cv::Mat structuringElement15x15 =
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));

    for (unsigned int i = 0; i < 2; i++) {
      cv::dilate(imgThresh, imgThresh, structuringElement5x5);
      cv::dilate(imgThresh, imgThresh, structuringElement5x5);
      cv::erode(imgThresh, imgThresh, structuringElement5x5);
    }

    cv::Mat imgThreshCopy = imgThresh.clone();

    std::vector<std::vector<cv::Point>> contours;

    cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);

    Drawer::drawAndShowContours(imgThresh.size(), contours, "imgContours");

    std::vector<std::vector<cv::Point>> convexHulls(contours.size());

    for (unsigned int i = 0; i < contours.size(); i++) {
      cv::convexHull(contours[i], convexHulls[i]);
    }

    Drawer::drawAndShowContours(imgThresh.size(), convexHulls,
                                "imgConvexHulls");

    for (auto &convexHull : convexHulls) {
      Blob possibleBlob(convexHull);

      if (possibleBlob.currentBoundingRect.area() > 400 &&
          possibleBlob.dblCurrentAspectRatio > 0.2 &&
          possibleBlob.dblCurrentAspectRatio < 4.0 &&
          possibleBlob.currentBoundingRect.width > 30 &&
          possibleBlob.currentBoundingRect.height > 30 &&
          possibleBlob.dblCurrentDiagonalSize > 60.0 &&
          (cv::contourArea(possibleBlob.currentContour) /
           (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
        currentFrameBlobs.push_back(possibleBlob);
      }
    }

    Drawer::drawAndShowContours(imgThresh.size(), currentFrameBlobs,
                                "imgCurrentFrameBlobs");

    if (blnFirstFrame == true) {
      for (auto &currentFrameBlob : currentFrameBlobs) {
        blobs.push_back(currentFrameBlob);
      }
    } else {
      Tools::matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
    }

    Drawer::drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");
    // get another copy of frame 2 since we changed the previous frame 2 copy in
    // the processing above
    imgFrame2Copy = imgFrame2.clone();

    Drawer::drawBlobInfoOnImage(blobs, imgFrame2Copy);

    int blnAtLeastOneBlobCrossedTheLine =
        traffic_monitor::Tools::checkIfBlobsCrossedTheLine(
            blobs, intVerticalLinePosition, carCountL, carCountR, myfile);

    if (blnAtLeastOneBlobCrossedTheLine == 1) {
      cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN,
               2);
    } else if (blnAtLeastOneBlobCrossedTheLine == 2) {
      cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_YELLOW,
               2);
    } else {
      cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_BLUE, 2);
    }

    Drawer::drawCarCountOnImage(carCountL, carCountR, imgFrame2Copy);

    cv::imshow("imgFrame2Copy", imgFrame2Copy);

    currentFrameBlobs.clear();

    // move frame 1 up to where frame 2 is
    imgFrame1 = imgFrame2.clone();

    capVideo.read(imgFrame2L);
    resize(imgFrame2L, imgFrame2,
           cv::Size(imgFrame2L.size().width / FRAME_SCALE,
                    imgFrame2L.size().height / FRAME_SCALE));

    blnFirstFrame = false;
    ++frameCount;
    chCheckForEscKey = cv::waitKey(1);
  }

  if (chCheckForEscKey != 27) {
    // if the user did not press esc (i.e. we reached the end of the video)
    // hold the windows open to allow the "end of video" message to show
    cv::waitKey(0);
  }
}

} /* namespace traffic_monitor */
