#!/bin/bash

# Компиляция с использованием стандарта C++17
g++ -std=c++17 cross_platform_timer.cpp -o cross_platform_timer -lpthread
g++ -std=c++17 child1.cpp -o child1
g++ -std=c++17 child2.cpp -o child2

# Запуск основной программы
./cross_platform_timer
