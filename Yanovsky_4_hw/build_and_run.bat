@echo off
REM Компиляция программы на C++
echo Compiling the C++ program...
g++ -o device_simulator device_simulator.cpp
g++ -o temperature_logger temperature_logger.cpp
if %errorlevel% neq 0 (
    echo Failed to compile the program.
    pause
    exit /b
)

REM Запуск симулятора устройства в фоновом режиме
echo Starting the device simulator...
start cmd /k device_simulator.exe

REM Ожидание, чтобы симулятор успел запуститься
timeout /t 2 >nul

REM Запуск программы логирования
echo Starting the temperature logger...
start cmd /k temperature_logger.exe

echo Device simulator and temperature logger are running.
pause