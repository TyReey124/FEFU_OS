@echo off
REM Update sources from Git
git pull origin main

REM Create build directory if not exists
if not exist build mkdir build
cd build

REM Build and compile
cmake .. -G "MinGW Makefiles"
cmake --build .

REM Run the program
task_1_2_project.exe
cd ..
