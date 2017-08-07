// (c) 2017 Vigilatore

#include <traffic_monitor/tracker.h>

namespace traffic_monitor {

Tracker::Tracker() {
  LOG(INFO) << "Setting up the tracker.";

  // capVideo.open("../movie.mp4");
  std::size_t num_cameras{0};
  for (std::size_t i = 0; i < 5; ++i) {
    vid_capture_.open(i);
    if (not vid_capture_.isOpened()) {
      ++num_cameras;
    }
  }
  if (num_cameras > 0) {
    LOG(INFO) << "There are " << num_cameras
              << " available. \nTaking the first one";
  }
}

int Tracker::run() {
  // TODO(gocarlos) choose the camera or video here.
  vid_capture_.open(0);

  log_file_.open("../logging/OpenCV-" + std::to_string(time(0)) + ".txt");
  std::cout << "Logging to: \"/tmp/OpenCV-"
            << "-" << std::to_string(time(0)) << ".txt\"" << std::endl;

  log_file_ << "\"Timestamp\",\"Left\",\"Right\"" << std::endl;

  vid_capture_.read(imgFrame1L);
  vid_capture_.read(imgFrame2L);

  resize(imgFrame1L, imgFrame1,
         cv::Size(imgFrame1L.size().width / Settings::frame_scale_,
                  imgFrame1L.size().height / Settings::frame_scale_));
  resize(imgFrame2L, imgFrame2,
         cv::Size(imgFrame2L.size().width / Settings::frame_scale_,
                  imgFrame2L.size().height / Settings::frame_scale_));

  vertical_line_position_ = std::round(imgFrame1.cols * 0.50f);

  crossing_line_[0].y = 0;
  crossing_line_[0].x = vertical_line_position_;

  crossing_line_[1].y = imgFrame1.rows - 1;
  crossing_line_[1].x = vertical_line_position_;

  frame_count_ = 2;
  bool first_frame = true;
  char chCheckForEscKey{0};

  LOG(INFO) << "Entering the main loop.";

  while (vid_capture_.isOpened() && chCheckForEscKey != 27) {
    std::vector<Blob> current_frame_blobs;

    cv::Mat imgFrame1Copy = imgFrame1.clone();
    cv::Mat imgFrame2Copy = imgFrame2.clone();

    cv::Mat img_diff;
    cv::Mat img_thresh;

    cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
    cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

    cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
    cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);

    cv::absdiff(imgFrame1Copy, imgFrame2Copy, img_diff);

    cv::threshold(img_diff, img_thresh, 30, 255.0, CV_THRESH_BINARY);

    cv::Mat structuringElement3x3 =
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat structuringElement5x5 =
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat structuringElement7x7 =
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
    cv::Mat structuringElement15x15 =
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));

    for (std::size_t i = 0; i < 2; ++i) {
      cv::dilate(img_thresh, img_thresh, structuringElement5x5);
      cv::dilate(img_thresh, img_thresh, structuringElement5x5);
      cv::erode(img_thresh, img_thresh, structuringElement5x5);
    }

    cv::Mat imgThreshCopy = img_thresh.clone();

    std::vector<std::vector<cv::Point>> contours;

    cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);

    Drawer::DrawAndShowContours(img_thresh.size(), contours, "imgContours");

    std::vector<std::vector<cv::Point>> convex_hulls(contours.size());

    for (unsigned int i = 0; i < contours.size(); i++) {
      cv::convexHull(contours[i], convex_hulls[i]);
    }

    Drawer::DrawAndShowContours(img_thresh.size(), convex_hulls,
                                "imgConvexHulls");

    for (auto &convexHull : convex_hulls) {
      Blob possible_blob(convexHull);

      // TODO(gocarlos) put those in the settings file.
      if (possible_blob.current_bounding_rect_.area() > 400 &&
          possible_blob.current_aspect_ratio_ > 0.2 &&
          possible_blob.current_aspect_ratio_ < 4.0 &&
          possible_blob.current_bounding_rect_.width > 30 &&
          possible_blob.current_bounding_rect_.height > 30 &&
          possible_blob.current_diagonal_size_ > 60.0 &&
          (cv::contourArea(possible_blob.current_contour_) /
           (double)possible_blob.current_bounding_rect_.area()) > 0.50) {
        current_frame_blobs.push_back(possible_blob);
      }
    }

    Drawer::DrawAndShowContours(img_thresh.size(), current_frame_blobs,
                                "imgCurrentFrameBlobs");

    if (first_frame == true) {
      for (auto &currentFrameBlob : current_frame_blobs) {
        blobs_.push_back(currentFrameBlob);
      }
    } else {
      Tools::MatchCurrentFrameBlobsToExistingBlobs(blobs_, current_frame_blobs);
    }

    Drawer::DrawAndShowContours(img_thresh.size(), blobs_, "imgBlobs");
    // Get another copy of frame 2 since we changed the previous frame 2 copy in
    // the processing above
    imgFrame2Copy = imgFrame2.clone();

    Drawer::DrawBlobInfoOnImage(blobs_, imgFrame2Copy);

    int at_least_one_blob_crossed_line =
        traffic_monitor::Tools::CheckIfBlobsCrossedTheLine(
            blobs_, vertical_line_position_, car_count_left, car_count_right,
            log_file_);

    if (at_least_one_blob_crossed_line == 1) {
      cv::line(imgFrame2Copy, crossing_line_[0], crossing_line_[1],
               SCALAR_GREEN, 2);
    } else if (at_least_one_blob_crossed_line == 2) {
      cv::line(imgFrame2Copy, crossing_line_[0], crossing_line_[1],
               SCALAR_YELLOW, 2);
    } else {
      cv::line(imgFrame2Copy, crossing_line_[0], crossing_line_[1], SCALAR_BLUE,
               2);
    }

    Drawer::DrawCarCountOnImage(car_count_left, car_count_right, imgFrame2Copy);

    cv::imshow("imgFrame2Copy", imgFrame2Copy);

    current_frame_blobs.clear();

    // move frame 1 up to where frame 2 is
    imgFrame1 = imgFrame2.clone();

    vid_capture_.read(imgFrame2L);
    resize(imgFrame2L, imgFrame2,
           cv::Size(imgFrame2L.size().width / Settings::frame_scale_,
                    imgFrame2L.size().height / Settings::frame_scale_));

    first_frame = false;
    ++frame_count_;
    chCheckForEscKey = cv::waitKey(1);
  }

  if (chCheckForEscKey != 27) {
    // If the user did not press ESC (i.e. we reached the end of the video)
    // hold the windows open to allow the "end of video" message to show.
    cv::waitKey(0);
  }
  return 0;
}

} /* namespace traffic_monitor */
