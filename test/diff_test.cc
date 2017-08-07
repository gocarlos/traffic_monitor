
#include “gtest/gtest.h”

#include <string>
#include <vector>

namespace traffic_monitor {

TEST(GetInitialPose, test_with_static_data) {

  double maximal_distance = 0.1;
  EXPECT_LE(0.1, maximal_distance);
}

}  // namespace traffic_monitor
