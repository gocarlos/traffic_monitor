
#include "gtest/gtest.h"

#include "traffic_monitor/settings.h"
#include "traffic_monitor/tracker.h"

#include <string>
#include <vector>

namespace traffic_monitor {

TEST(GetInitialPose, test_with_static_data) {
  Tracker tracker;

  tracker.input_ = Tracker::file;
  tracker.video_path_ = "movie.mp4";

  tracker.run();
  double maximal_distance = 0.1;
  EXPECT_LE(0.1, maximal_distance);
}

}  // namespace traffic_monitor
