#!/bin/bash
# Update sources from Git
git pull https://github.com/TyReey124/FEFU_OS/tree/main/Yanovsky_1_hw main

# Create build directory if not exists
mkdir -p build
cd build

# Build and compile
cmake ..
make

# Run the program
./task_1_2_project
cd ..
