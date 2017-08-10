/*
 * statistics.h
 *
 *  Created on: Aug 10, 2017
 *      Author: gocarlos
 */

#ifndef INCLUDE_TRAFFIC_MONITOR_STATISTICS_H_
#define INCLUDE_TRAFFIC_MONITOR_STATISTICS_H_

namespace traffic_monitor {

class Statistics {
 public:
  Statistics();
  virtual ~Statistics();
  static int car_count_left_;
  static int car_count_right_;
};

} /* namespace traffic_monitor */

#endif /* INCLUDE_TRAFFIC_MONITOR_STATISTICS_H_ */
