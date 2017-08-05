// (c) 2017 Vigilatore

#include "traffic_monitor/tracker.h"

using namespace traffic_monitor;

int main(int argc, char *argv[]) {
  google::InitGoogleLogging(argv[0]);

  Tracker tracker;

  return tracker.run();
}
