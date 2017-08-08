#!/bin/bash

# python3 -c "
# import os
# for root, dirs, files in os.walk('.', topdown=True):
#     dirs.clear() #with topdown true, this will prevent walk from going into subs
#     for file in files:
#       #do some stuff
#       print(file)
# "

python3 test/get_test_data.py
./build/traffic_monitor_tests
