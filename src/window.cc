//// (c) 2017 TrafficMonitor
//
//#include <QtWidgets>
//
//#include "ui_window.h"
//
//#include "traffic_monitor/window.h"
//#include "glog/logging.h"
//
//Window::Window(QWidget *parent) : QWidget(parent), ui(new Ui::Window) {
//  ui->setupUi(this);
//  ui->text_label->setText(
//      "SPACE to start and stop the Engine. \nESC to close the window. ");
//
//  LOG(INFO) << "Starting the engine...";
//  timerId = startTimer(50);
//}
//
//Window::~Window() {
//  killTimer(timerId);
//  delete ui;
//}
//
//void Window::timerEvent(QTimerEvent *event) {
//  // Show the image in a Qlabel.
////  cv::Mat img2;
////  cvtColor(img2, img2, CV_BGR2RGB);
////  ui->image_label->setPixmap(QPixmap::fromImage(QImage(
////      img2.data, img2.cols, img2.rows, img2.step, QImage::Format_RGB888)));
//}
//
//void Window::keyPressEvent(QKeyEvent *e) {
//  if (e->key() == Qt::Key_Escape) {
//    // Close the window.
//    close();
//  } else if (e->key() == Qt::Key_Space) {
//    // Start or stop tracking.
//
//    LOG(WARNING) << "ESC pressed.";
//
//  } else {
//    QWidget::keyPressEvent(e);
//  }
//}
