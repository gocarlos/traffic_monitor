CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

SET(TRAFFIC_MONITOR "TrafficMonitor")
# SET(TRAFFIC_MONITOR_TARGET "traffic_monitor")

IF(NOT PROJECT_NAME)
  PROJECT(${TRAFFIC_MONITOR})
ENDIF(NOT PROJECT_NAME)


ADD_COMPILE_OPTIONS("-std=c++17")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -w")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

FIND_PACKAGE(OpenCV REQUIRED)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${CMAKE_CURRENT_LIST_DIR}/external/miniglog
  ${OpenCV_INCLUDE_DIRS}
)
# TODO(gocarlos) add this as external project.
INCLUDE("${CMAKE_CURRENT_LIST_DIR}/external/miniglog/CMakeLists.txt")

SET(TM_LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
SET(TM_LIB_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

################################
# Define Traffic Monitor Ressources
################################

FILE(GLOB_RECURSE TM_LIB_HEADERS "${TM_LIB_HEADER_DIR}/*.h")
FILE(GLOB_RECURSE TM_LIB_SOURCES "${TM_LIB_SRC_DIR}/*.cc")

# Remove the executable from that list.
SET(TM_LIB_RESOURCES ${TM_LIB_SOURCES} ${TM_LIB_HEADERS})
LIST(REMOVE_ITEM TM_LIB_RESOURCES "${TM_LIB_SRC_DIR}/main.cc")

################################
# Define Traffic Monitor Executable
################################

ADD_EXECUTABLE(traffic_monitor ${TM_LIB_RESOURCES} "${TM_LIB_SRC_DIR}/main.cc")
TARGET_LINK_LIBRARIES(traffic_monitor
  ${TM_LIB} ${OpenCV_LIBRARIES} ${MINI_GLOG}
)

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
