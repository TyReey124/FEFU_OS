# Требования
- GCC, MinGW или MSVC.
- CMake
- Make (Linux/Unix) или MSBuild/Ninja (Windows).

# Запуск

## Linux
- `chmod +x build_and_run.sh` - сделать скрипт исполняемым
- `./build_and_run.sh` - для запуска

## Windows
- `build_and_run.bat` - для запуска, если есть MinGW

# Файлы проекта
- "process_launcher.h" и "process_launcher.cpp" Реализация библиотеки для запуска процессов.
- "test_process_launcher.cpp" Тестовая утилита для демонстрации библиотеки.
- "CMakeLists.txt" Конфигурация для сборки проекта.
- "build_and_run.sh" Скрипт автоматизации для Linux/Unix.
- "build_and_run.bat" Скрипт автоматизации для Windows.

# Реализация проекта
- На Windows запускает ping 127.0.0.1 -n 3.
- На Linux запускает sleep 3.
