

# TODO add gtest as external project here. 

#INCLUDE(ExternalProject)
#
#externalproject_add(googletest
#  GIT_REPOSITORY https://github.com/google/googletest
#  SOURCE_DIR googletest
#  BUILD_IN_SOURCE 1
#  UPDATE_COMMAND ""
#  PATCH_COMMAND ""
#  CMAKE_ARGS
#    -DCMAKE_BUILD_TYPE:STRING=Release
#    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/thirdparty
#    -Dgtest_force_shared_crt=OFF
#  INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty
#  INSTALL_COMMAND ""
#)
#
#set(GOOGLETEST_INCLUDE_DIR "${CMAKE_BINARY_DIR}/googletest/googletest/include/" "${CMAKE_BINARY_DIR}/googletest/googlemock/include/")
#set(GOOGLETEST_LIBRARY_DIR ${CMAKE_BINARY_DIR}/googletest/)
#
#include_directories(${GOOGLETEST_INCLUDE_DIR})
#link_directories(${GOOGLETEST_LIBRARY_DIR})
#
#set(GOOGLETEST_LIBRARIES "${GOOGLETEST_LIBRARY_DIR}googlemock/libgmock.a" "${GOOGLETEST_LIBRARY_DIR}googlemock/libgmock_main.a")

########################### GTEST
# Enable ExternalProject CMake module
#INCLUDE(ExternalProject)
#
# Set default ExternalProject root directory
#set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/external)
#
# Add gtest
# http://stackoverflow.com/questions/9689183/cmake-googletest
#externalproject_add(
#    googletest
#    URL http://github.com/google/googletest/archive/release-1.8.0.tar.gz
    # Disable install step
#    INSTALL_COMMAND ""
    # Wrap download, configure and build steps in a script to log output
#    LOG_DOWNLOAD ON
#    LOG_CONFIGURE ON
#    LOG_BUILD ON)
#
# Specify include dir
#externalproject_get_property(googletest source_dir)
#set(GTEST_INCLUDE_DIRS ${source_dir}/googletest/include)
#
# Library
#externalproject_get_property(googletest binary_dir)
#set(GTEST_LIBRARIES ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a)
#add_library(gtest UNKNOWN IMPORTED)
#set_property(TARGET gtest PROPERTY IMPORTED_LOCATION
#             ${GTEST_LIBRARIES})
#set_property(TARGET googletest PROPERTY EXCLUDE_FROM_ALL 1)
#add_dependencies(gtest googletest)
#
