#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <fstream>

int main() {
    std::srand(std::time(0)); // Инициализация генератора случайных чисел

    while (true) {
        std::ofstream comPort("com_port.txt", std::ios::app);

        if (!comPort.is_open()) {
            std::cerr << "Ошибка открытия файла com_port.txt!" << std::endl;
            return 1;
        }

        int temperature = 20 + std::rand() % 10; // Генерация случайной температуры (от 20 до 29)
        comPort << temperature << std::endl;     // Запись температуры в файл
        comPort.flush();                         // Очистка буфера
        comPort.close();                         // Закрываем файл после записи

        sleep(1);                                // Пауза 1 секунда
    }

    return 0;
}
