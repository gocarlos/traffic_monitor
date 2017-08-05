/*
 * tracker.h
 *
 *  Created on: Aug 5, 2017
 *      Author: gocarlos
 */

#ifndef INCLUDE_TRAFFIC_MONITOR_TRACKER_H_
#define INCLUDE_TRAFFIC_MONITOR_TRACKER_H_

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
namespace traffic_monitor {

class Tracker {
 public:
  Tracker();
  virtual ~Tracker();
  int run();
};

} /* namespace traffic_monitor */

#endif /* INCLUDE_TRAFFIC_MONITOR_TRACKER_H_ */
