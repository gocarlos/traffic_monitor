#!/bin/bash

rm -rf build/

# Build traffic_monitor
mkdir -p build
cd build
cmake .. -DBUILD_TESTS=TRUE -DDO_COVERAGE_TEST=TRUE
make -j4
