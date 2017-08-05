// (c) 2017 Vigilatore

#include <traffic_monitor/drawer.h>

namespace traffic_monitor {

Drawer::Drawer() {
  // TODO Auto-generated constructor stub
}

Drawer::~Drawer() {
  // TODO Auto-generated destructor stub
}

void Drawer::drawBlobInfoOnImage(std::vector<Blob> &blobs,
                                 cv::Mat &imgFrame2Copy) {
  for (unsigned int i = 0; i < blobs.size(); i++) {
    if (blobs[i].blnStillBeingTracked == true) {
      cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_RED, 2);

      int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
      double dblFontScale = blobs[i].dblCurrentDiagonalSize / 60.0;
      int intFontThickness = (int)std::round(dblFontScale * 1.0);

      cv::putText(imgFrame2Copy, std::to_string(i),
                  blobs[i].centerPositions.back(), intFontFace, dblFontScale,
                  SCALAR_GREEN, intFontThickness);
    }
  }
}

void Drawer::drawCarCountOnImage(int &carCountL, int &carCountR,
                                 cv::Mat &imgFrame2Copy) {
  int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
  double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 300000.0;
  int intFontThickness = (int)std::round(dblFontScale * 1.5);

  cv::Size textSizeL =
      cv::getTextSize("L: " + std::to_string(carCountL), intFontFace,
                      dblFontScale, intFontThickness, 0);
  cv::Size textSizeR =
      cv::getTextSize("R: " + std::to_string(carCountR), intFontFace,
                      dblFontScale, intFontThickness, 0);

  cv::Point ptTextBottomLeftPositionL, ptTextBottomLeftPositionR;

  ptTextBottomLeftPositionL.x =
      imgFrame2Copy.cols - 1 - (int)((double)textSizeL.width * 1.25);
  ptTextBottomLeftPositionL.y = (int)((double)textSizeL.height * 1.25);

  ptTextBottomLeftPositionR.x = ptTextBottomLeftPositionL.x;
  ptTextBottomLeftPositionR.y =
      ptTextBottomLeftPositionL.y + (textSizeL.height) * 1.25;

  cv::putText(imgFrame2Copy, "L: " + std::to_string(carCountL),
              ptTextBottomLeftPositionL, intFontFace, dblFontScale,
              SCALAR_GREEN, intFontThickness);
  cv::putText(imgFrame2Copy, "R: " + std::to_string(carCountR),
              ptTextBottomLeftPositionR, intFontFace, dblFontScale,
              SCALAR_YELLOW, intFontThickness);
}

void Drawer::drawAndShowContours(cv::Size imageSize,
                                 std::vector<std::vector<cv::Point>> contours,
                                 std::string strImageName) {
  cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

  cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);

  // cv::imshow(strImageName, image);
}

void Drawer::drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs,
                                 std::string strImageName) {
  cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

  std::vector<std::vector<cv::Point>> contours;

  for (auto &blob : blobs) {
    if (blob.blnStillBeingTracked == true) {
      contours.push_back(blob.currentContour);
    }
  }

  cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);

  // cv::imshow(strImageName, image);
}

} /* namespace traffic_monitor */
