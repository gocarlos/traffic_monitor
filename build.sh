#!/bin/bash

rm -rf build/

# Build traffic_monitor
mkdir -p build
cd build
cmake ..   -DBUILD_TESTS=TRUE
make -j4
