#!/bin/bash
mkdir -p build
cd build

cmake ..
make

touch com_port.txt

./device_simulator &
./temperature_logger

kill $!
