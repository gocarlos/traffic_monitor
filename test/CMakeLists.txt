################################
# Google Test Setup
################################

# When building OpenMapper with ROS, gtest_main & gtest are already targets which are introduced automatically by catkin.
# Here gtest is only built if there is not target called gtest_main, which means catkin is not providing already this target.
# Similiar problem: https://stackoverflow.com/questions/42550873/compile-gtest-from-source-with-catkin
# See documentation for policy CMP0002.

IF(NOT TARGET ${gtest_main})
  IF(NOT BUILD_ROS_BINDING)
     ADD_SUBDIRECTORY(${CMAKE_CURRENT_LIST_DIR}/../external/googletest/googletest gtest_build)
    ENABLE_TESTING()
    INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  ENDIF(NOT BUILD_ROS_BINDING)
ENDIF(NOT TARGET ${gtest_main})

################################
# Traffic Monitor Tests
################################

SET(TM_TESTS "traffic_monitor_tests")

FILE(GLOB_RECURSE TM_TESTS_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")
FILE(GLOB_RECURSE TM_TESTS_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cc")

# Add test src files
ADD_EXECUTABLE(${TM_TESTS}
  ${TM_TESTS_HEADERS}
  ${TM_TESTS_SOURCES}
  ${TM_LIB_RESOURCES}
  
)

# Link test executable against gtest & gtest_main
TARGET_LINK_LIBRARIES(${TM_TESTS}
  gtest gtest_main
  ${TM_EXTERNAL_RESOURCES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  ${OPENSSL_LIBRARIES}
)

ADD_TEST(${TM_TESTS} ${TM_TESTS})

TARGET_INCLUDE_DIRECTORIES(${TM_TESTS} PUBLIC include)