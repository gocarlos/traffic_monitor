CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

SET(TRAFFIC_MONITOR "TrafficMonitor")
IF(NOT PROJECT_NAME)
  PROJECT(${TRAFFIC_MONITOR})
ENDIF(NOT PROJECT_NAME)


ADD_COMPILE_OPTIONS("-std=c++17")
OPTION(DO_COVERAGE_TEST "Test the coverage of the unit tests using CodeCov" OFF)


# Instruct CMake to run moc automatically when needed.
#SET(CMAKE_AUTOMOC ON)
#SET(CMAKE_AUTOUIC ON)
# Find includes in corresponding build directories.
#SET(CMAKE_INCLUDE_CURRENT_DIR ON)


SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -w")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_CURRENT_LIST_DIR}/external)

FIND_PACKAGE(OpenCV REQUIRED)
#FIND_PACKAGE(Qt5Widgets REQUIRED)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_LIST_DIR}/include
#  ${CMAKE_CURRENT_LIST_DIR}/include/traffic_monitor

  ${CMAKE_CURRENT_LIST_DIR}/external/miniglog
  ${CMAKE_CURRENT_LIST_DIR}/external/argparse
  ${CMAKE_CURRENT_LIST_DIR}/external/cxxopts/include
  ${OpenCV_INCLUDE_DIRS}
)
# TODO(gocarlos) add this as external project.
INCLUDE("${CMAKE_CURRENT_LIST_DIR}/external/miniglog/CMakeLists.txt")

SET(TM_LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
SET(TM_LIB_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)


IF(DO_COVERAGE_TEST)
  MESSAGE("\nGoing to build the coverage tests.\n\n\n")
  INCLUDE(CodeCoverage)
  SETUP_TARGET_FOR_COVERAGE(${PROJECT_NAME}_coverage tests coverage)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
ELSE()
  MESSAGE("\nNot building the coverage tests. \n\n\n")

  # Optimize the code with the compiler if the target device has a x86 processor and not doing code coverage tests.
  IF(CMAKE_SYSTEM_PROCESSOR MATCHES "amd64.*|x86_64.*|AMD64.*|i686.*|i386.*|x86.*")
    MESSAGE("Current System is x86.")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
  ENDIF()

  IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
  ENDIF()

ENDIF()

################################
# Define Traffic Monitor Ressources
################################

FILE(GLOB_RECURSE TM_LIB_HEADERS "${TM_LIB_HEADER_DIR}/*.h ")
FILE(GLOB_RECURSE TM_LIB_SOURCES "${TM_LIB_SRC_DIR}/*.cc")

# Remove the executable from that list.
SET(TM_LIB_RESOURCES ${TM_LIB_SOURCES} ${TM_LIB_HEADERS})
LIST(REMOVE_ITEM TM_LIB_RESOURCES "${TM_LIB_SRC_DIR}/main.cc")


SET(TM_EXTERNAL_RESOURCES
  ${OpenCV_LIBRARIES}
  ${MINI_GLOG}
#  Qt5::Widgets
#  Qt5::Core
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
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt")
ENDIF(BUILD_TESTS)

################################
# Install Traffic Monitor Executable
################################

INSTALL(
  TARGETS traffic_monitor
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
)

INSTALL(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/settings DESTINATION traffic_monitor)
