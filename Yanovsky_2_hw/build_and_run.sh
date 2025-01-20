#!/bin/bash

# Создание папки сборки
mkdir -p build && cd build

# Генерация файлов сборки с помощью CMake
cmake .. || { echo "CMake configuration failed"; exit 1; }

# Сборка проекта
make || { echo "Build failed"; exit 1; }

# Запуск тестовой программы
./test_process_launcher || { echo "Execution failed"; exit 1; }
