#!/bin/bash
# Update sources from Git
git pull https://github.com/TyReey124/FEFU_OS main

# Create build directory if not exists
mkdir -p build
cd build

# Build and compile
cmake ..
make

# Run the program
./task_1_2_project
cd ..
