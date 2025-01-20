#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <unistd.h>
#include <deque>

std::string getCurrentTimestamp() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return std::string(buf);
}

void cleanupLogs(std::deque<std::string>& log, int maxEntries) {
    if (log.size() > maxEntries) {
        log.pop_front();
    }
}

int main() {
    std::deque<std::string> allTempsLog;
    std::deque<std::string> hourlyAvgLog;
    std::deque<std::string> dailyAvgLog;

    std::ofstream logAll("all_temps.log", std::ios::app);
    std::ofstream logHourly("hourly_avg.log", std::ios::app);
    std::ofstream logDaily("daily_avg.log", std::ios::app);

    std::vector<int> hourlyTemps;
    std::vector<int> dailyTemps;
    int secondCounter = 0;
    int minuteCounter = 0;

    while (true) {
        std::ifstream comPort("com_port.txt");
        if (!comPort.is_open()) {
            std::cerr << "Ошибка открытия файла com_port.txt!" << std::endl;
            return 1;
        }

        std::string line;
        std::string lastLine;
        while (std::getline(comPort, line)) {
            lastLine = line;
        }

        if (!lastLine.empty()) {
            int temp = std::stoi(lastLine);
            std::string timestamp = getCurrentTimestamp();

            // Запись в all_temps.log
            allTempsLog.push_back(timestamp + " " + std::to_string(temp));
            cleanupLogs(allTempsLog, 1440); // Храним только последние 1440 записей
            logAll << allTempsLog.back() << std::endl;
            logAll.flush();

            // Сбор данных для hourly_avg.log
            hourlyTemps.push_back(temp);
            if (++secondCounter >= 60) { // Каждую минуту
                int sum = 0;
                for (int t : hourlyTemps) sum += t;
                int avg = sum / hourlyTemps.size();
                hourlyAvgLog.push_back(timestamp + " " + std::to_string(avg));
                cleanupLogs(hourlyAvgLog, 720); // Храним только последние 720 записей
                logHourly << hourlyAvgLog.back() << std::endl;
                logHourly.flush(); // Очищаем буфер

                hourlyTemps.clear();
                secondCounter = 0;

                // Сбор данных для daily_avg.log
                dailyTemps.push_back(avg);
                if (++minuteCounter >= 24) { // Каждые 24 минуты
                    sum = 0;
                    for (int t : dailyTemps) sum += t;
                    avg = sum / dailyTemps.size();
                    dailyAvgLog.push_back(timestamp + " " + std::to_string(avg));
                    cleanupLogs(dailyAvgLog, 8760); // Храним только последние 8760 записей
                    logDaily << dailyAvgLog.back() << std::endl;
                    logDaily.flush(); // Очищаем буфер

                    dailyTemps.clear();
                    minuteCounter = 0;
                }
            }
        }

        comPort.close(); // Закрываем файл после чтения
        sleep(1); // Пауза 1 секунда
    }

    return 0;
}
