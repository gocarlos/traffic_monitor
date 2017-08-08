CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

SET(TRAFFIC_MONITOR "TrafficMonitor")

IF(NOT PROJECT_NAME)
  PROJECT(${TRAFFIC_MONITOR})
ENDIF(NOT PROJECT_NAME)

ADD_COMPILE_OPTIONS("-std=c++17")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -w")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_CURRENT_LIST_DIR}/external)

FIND_PACKAGE(OpenCV REQUIRED)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${CMAKE_CURRENT_LIST_DIR}/external/miniglog
  ${CMAKE_CURRENT_LIST_DIR}/external/argparse
  ${CMAKE_CURRENT_LIST_DIR}/external/cxxopts/include
  ${OpenCV_INCLUDE_DIRS}
)
# TODO(gocarlos) add this as external project.
INCLUDE("${CMAKE_CURRENT_LIST_DIR}/external/miniglog/CMakeLists.txt")

SET(TM_LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
SET(TM_LIB_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

################################
# Define Traffic Monitor Ressources
################################

FILE(GLOB_RECURSE TM_LIB_HEADERS "${TM_LIB_HEADER_DIR}/*.h ")
FILE(GLOB_RECURSE TM_LIB_SOURCES "${TM_LIB_SRC_DIR}/*.cc")

# Remove the executable from that list.
SET(TM_LIB_RESOURCES ${TM_LIB_SOURCES} ${TM_LIB_HEADERS})
LIST(REMOVE_ITEM TM_LIB_RESOURCES "${TM_LIB_SRC_DIR}/main.cc")


SET(TM_EXTERNAL_RESOURCES ${OpenCV_LIBRARIES} ${MINI_GLOG}
)

# todo(gocarlos) add here a library

################################
# Define Traffic Monitor Executable
################################

ADD_EXECUTABLE(traffic_monitor ${TM_LIB_RESOURCES} "${TM_LIB_SRC_DIR}/main.cc")
TARGET_LINK_LIBRARIES(traffic_monitor
  ${TM_EXTERNAL_RESOURCES}
)

################################
# Define Unit Tests
################################

IF(BUILD_TESTS)

  SET(TM_TESTS "traffic_monitor_tests")

  ENABLE_TESTING()
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/cmake/FindGoogleTest.cmake")
  INCLUDE_DIRECTORIES(
    ${gtest_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}
    )

  FILE(GLOB_RECURSE TM_TESTS_HEADERS "${CMAKE_CURRENT_LIST_DIR}/test/*.h")
  FILE(GLOB_RECURSE TM_TESTS_SOURCES "${CMAKE_CURRENT_LIST_DIR}/test/*.cc")

  # Add test src files
  ADD_EXECUTABLE(${TM_TESTS}
    ${TM_TESTS_HEADERS}
    ${TM_TESTS_SOURCES}
    ${TM_LIB_RESOURCES}
  )

 TARGET_LINK_LIBRARIES(${TM_TESTS}
#  ${OpenCV_LIBRARIES} ${MINI_GLOG}
    ${TM_EXTERNAL_RESOURCES}
  
  gtest pthread
  )

  ADD_TEST(${TM_TESTS}
    ${TM_TESTS_HEADERS}
    ${TM_TESTS_SOURCES}
    ${TM_LIB_RESOURCES}
  )

ENDIF(BUILD_TESTS)

################################
# Install Traffic Monitor Executable
################################

INSTALL(
   TARGETS traffic_monitor
   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   )

# TODO(gocarlos) put the settings files there
# INSTALL(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/data/ DESTINATION traffic_monitor)
