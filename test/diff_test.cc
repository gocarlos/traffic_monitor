
#include "gtest/gtest.h"

#include "traffic_monitor/settings.h"
#include "traffic_monitor/tracker.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace traffic_monitor {

TEST(IntegrationTests, tes_with_video_data) {
  Tracker tracker;
  std::string path_to_file_ = "movie.mp4";

  std::ifstream f(path_to_file_.c_str());
  CHECK(f.good());

  tracker.input_ = Tracker::file;
  tracker.video_path_ = path_to_file_;

  tracker.run();
  double maximal_distance = 0.1;
  EXPECT_LE(0.1, maximal_distance);
}

}  // namespace traffic_monitor
