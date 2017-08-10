
#include "gtest/gtest.h"

#include "traffic_monitor/settings.h"
#include "traffic_monitor/tracker.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace traffic_monitor {

TEST(IntegrationTests, tes_with_video_data) {
  std::string stream = "movie.mp4";
  // Check if the file exists.
  std::ifstream f(stream.c_str());
  CHECK(f.good());

  Tracker tracker(Tracker::file, stream);

  // Travis supports testing of gui apps :
  // http://craigthomas.ca/blog/2014/05/26/travis-ci-and-gui-testing/
  Settings::with_gui_ = true;

  tracker.RunTracker();

  double maximal_distance = 0.1;
  EXPECT_LE(0.1, maximal_distance);
}

}  // namespace traffic_monitor
