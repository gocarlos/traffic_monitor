// (c) 2017 Vigilatore

#include <stdio.h>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "traffic_monitor/blob.h"
#include "traffic_monitor/drawer.h"
#include "traffic_monitor/tools.h"

using namespace traffic_monitor;

#define SHOW_STEPS  // un-comment or comment this line to show steps or not

#define FRAME_SCALE 1  // divide frame dimentions by this number

// global variables
// ///////////////////////////////////////////////////////////////////////////////


// void drawAndShowContours(cv::Size imageSize,
//                         std::vector<std::vector<cv::Point>> contours,
//                         std::string strImageName);
// void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs,
////                         std::string strImageName);
// bool checkIfBlobsCrossedTheLine(std::vector<Blob> &blobs,
//                                int &intVerticalLinePosition, int &carCountL,
//                                int &carCountR, std::ofstream &myfile);
// void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy);
// void drawCarCountOnImage(int &carCountL, int &carCountR,
//                         cv::Mat &imgFrame2Copy);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
  google::InitGoogleLogging(argv[0]);

  cv::VideoCapture capVideo;
  std::ofstream myfile;

  cv::Mat imgFrame1;
  cv::Mat imgFrame2;
  cv::Mat imgFrame1L;
  cv::Mat imgFrame2L;

  std::vector<Blob> blobs;

  cv::Point crossingLine[2];

  int carCountL = 0;
  int carCountR = 0;

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

  // int intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows *
  // 0.35);
  int intVerticalLinePosition = (int)std::round((double)imgFrame1.cols * 0.50);

  crossingLine[0].y = 0;
  crossingLine[0].x = intVerticalLinePosition;

  crossingLine[1].y = imgFrame1.rows - 1;
  crossingLine[1].x = intVerticalLinePosition;

  char chCheckForEscKey = 0;

  bool blnFirstFrame = true;

  int frameCount = 2;

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

    imgFrame2Copy = imgFrame2.clone();  // get another copy of frame 2 since we
                                        // changed the previous frame 2 copy in
                                        // the processing above

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

    imgFrame1 = imgFrame2.clone();  // move frame 1 up to where frame 2 is

    capVideo.read(imgFrame2L);
    resize(imgFrame2L, imgFrame2,
           cv::Size(imgFrame2L.size().width / FRAME_SCALE,
                    imgFrame2L.size().height / FRAME_SCALE));

    blnFirstFrame = false;
    frameCount++;
    chCheckForEscKey = cv::waitKey(1);
  }

  if (chCheckForEscKey != 27) {  // if the user did not press esc (i.e. we
                                 // reached the end of the video)
    cv::waitKey(0);  // hold the windows open to allow the "end of video"
                     // message to show
  }
  // note that if the user did press esc, we don't need to hold the windows
  // open, we can simply let the program end which will close the windows

  return (0);
}


