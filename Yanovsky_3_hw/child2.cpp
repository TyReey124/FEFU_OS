#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#define getpid() GetCurrentProcessId()
#else
#include <unistd.h>
#include <sys/file.h>
#endif

std::mutex log_mutex;

void log_message(const std::string &message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::ofstream log_file("program.log", std::ios::app);
    if (log_file.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::tm tm = *std::localtime(&time_t_now);
        log_file << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "." << ms.count()
                 << " [PID: " << getpid() << "] " << message << std::endl;
    }
}

// Функция для блокировки файла
bool lock_file(int fd) {
#ifdef _WIN32
    HANDLE hFile = (HANDLE)_get_osfhandle(fd);
    if (hFile == INVALID_HANDLE_VALUE) return false;
    OVERLAPPED overlapped = {0};
    return LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, 1, 0, &overlapped);
#else
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    return fcntl(fd, F_SETLK, &fl) != -1;
#endif
}

// Функция для разблокировки файла
bool unlock_file(int fd) {
#ifdef _WIN32
    HANDLE hFile = (HANDLE)_get_osfhandle(fd);
    if (hFile == INVALID_HANDLE_VALUE) return false;
    OVERLAPPED overlapped = {0};
    return UnlockFileEx(hFile, 0, 1, 0, &overlapped);
#else
    struct flock fl;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    return fcntl(fd, F_SETLK, &fl) != -1;
#endif
}

int main() {
    log_message("Child2 started.");

    // Блокируем файл перед чтением
    int fd = open("counter.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        log_message("Failed to open counter.txt");
        return 1;
    }

    if (lock_file(fd)) {
        std::ifstream counter_file("counter.txt");
        int counter_value;
        counter_file >> counter_value;
        counter_file.close();

        // Увеличение счетчика в 2 раза
        counter_value *= 2;
        log_message("Counter increased to: " + std::to_string(counter_value));

        // Запись нового значения счетчика в файл
        std::ofstream counter_file_out("counter.txt");
        counter_file_out << counter_value;
        counter_file_out.close();

        unlock_file(fd);
    } else {
        log_message("Failed to lock counter.txt");
    }

    close(fd);

    // Ожидание 2 секунды
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Блокируем файл перед чтением
    fd = open("counter.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        log_message("Failed to open counter.txt");
        return 1;
    }

    if (lock_file(fd)) {
        std::ifstream counter_file("counter.txt");
        int counter_value;
        counter_file >> counter_value;
        counter_file.close();

        // Уменьшение счетчика в 2 раза
        counter_value /= 2;
        log_message("Counter decreased to: " + std::to_string(counter_value));

        // Запись нового значения счетчика в файл
        std::ofstream counter_file_out("counter.txt");
        counter_file_out << counter_value;
        counter_file_out.close();

        unlock_file(fd);
    } else {
        log_message("Failed to lock counter.txt");
    }

    close(fd);

    log_message("Child2 completed work and exited.");

    return 0;
}
