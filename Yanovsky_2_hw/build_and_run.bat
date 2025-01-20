@echo off
rem Создание папки сборки
if not exist build mkdir build
cd build

rem Генерация файлов сборки с помощью CMake
cmake .. || goto :cmake_failed

rem Сборка проекта
cmake --build . || goto :build_failed

rem Запуск тестовой программы
test_process_launcher.exe || goto :execution_failed
goto :end

:cmake_failed
echo CMake configuration failed
exit /b 1

:build_failed
echo Build failed
exit /b 1

:execution_failed
echo Execution failed
exit /b 1

:end
