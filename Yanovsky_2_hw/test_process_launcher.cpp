#include "process_launcher.h"
#include <iostream>

int main() {
    // Пример запуска программы, которая выполняется дольше
#ifdef _WIN32
    std::string command = "ping 127.0.0.1 -n 3"; // Пинг на 3 пакета в Windows
#else
    std::string command = "sleep 3"; // Пауза 3 секунды в Linux/Unix
#endif

    std::cout << "Starting process: " << command << std::endl;
    auto result = process_launcher::run(command, true); // Ожидаем завершения
    if (result) {
        std::cout << "Process completed successfully with code: " << *result << std::endl;
    } else {
        std::cout << "Failed to execute process." << std::endl;
    }

    return 0;
}
