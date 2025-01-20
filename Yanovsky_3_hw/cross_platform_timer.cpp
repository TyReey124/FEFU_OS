#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#define getpid() GetCurrentProcessId()
#else
#include <unistd.h>
#include <sys/file.h>
#endif

std::mutex log_mutex;
std::ofstream log_file;
std::atomic<int> counter(0);
bool is_master = false;

// Функция для логирования сообщений
void log_message(const std::string &message) {
    std::lock_guard<std::mutex> lock(log_mutex);
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

// Чтение счетчика из файла
int read_counter() {
    int fd = open("counter.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        log_message("Failed to open counter.txt");
        return -1;
    }

    int counter_value = 0;
    if (lock_file(fd)) {
        std::ifstream counter_file("counter.txt");
        counter_file >> counter_value;
        counter_file.close();
        unlock_file(fd);
    } else {
        log_message("Failed to lock counter.txt");
    }

    close(fd);
    return counter_value;
}

// Запись счетчика в файл
void write_counter(int value) {
    int fd = open("counter.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        log_message("Failed to open counter.txt");
        return;
    }

    if (lock_file(fd)) {
        std::ofstream counter_file("counter.txt");
        counter_file << value;
        counter_file.close();
        unlock_file(fd);
    } else {
        log_message("Failed to lock counter.txt");
    }

    close(fd);
}

// Таймер для увеличения счетчика каждые 300 мс
void timer_300ms() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        // Чтение текущего значения счетчика из файла
        int counter_value = read_counter();
        if (counter_value == -1) continue;

        // Увеличение счетчика
        counter_value++;
        write_counter(counter_value);
    }
}

// Логирование состояния счетчика каждую секунду
void log_status_every_second() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Чтение текущего значения счетчика из файла
        int counter_value = read_counter();
        if (counter_value == -1) continue;

        // Логирование
        log_message("Counter value: " + std::to_string(counter_value));
    }
}

// Запуск дочерних процессов
void spawn_child(const std::string &child_type) {
#ifdef _WIN32
    std::string command = "start " + child_type;
    system(command.c_str());
#else
    if (fork() == 0) {
        if (child_type == "child1") {
            execl("./child1", "child1", nullptr);
        } else if (child_type == "child2") {
            execl("./child2", "child2", nullptr);
        }
        exit(0);
    }
#endif
}

// Управление дочерними процессами
void manage_children() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        // Запуск child1
        log_message("Launching child1");
        spawn_child("child1");

        // Ждем завершения child1
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Запуск child2
        log_message("Launching child2");
        spawn_child("child2");
    }
}

// Обработка пользовательского ввода
void user_input() {
    while (true) {
        int new_value;
        std::cout << "Enter a new value for the counter: ";
        std::cin >> new_value;

        // Запись нового значения счетчика в файл
        write_counter(new_value);
        log_message("Counter set to: " + std::to_string(new_value));
    }
}

// Функция для определения, является ли процесс мастером
bool acquire_master_lock() {
#ifdef _WIN32
    HANDLE hMutex = CreateMutex(NULL, TRUE, "Global\\MasterProcessMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(hMutex);
        return false;
    }
    return true;
#else
    int fd = open("/tmp/master_lock", O_CREAT | O_RDWR, 0666);
    if (fd == -1) return false;

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        close(fd);
        return false; // Не удалось захватить блокировку
    }
    return true; // Успешно захватили блокировку
#endif
}

int main() {
    // Открываем лог-файл (создаем его, если он не существует)
    log_file.open("program.log", std::ios::app);
    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        return 1;
    }

    // Пытаемся открыть лог-файл в текстовом редакторе
    #ifdef __APPLE__
        system("open program.log");
    #elif _WIN32
        system("start program.log");
    #else
        system("xdg-open program.log");
    #endif

    // Определяем, является ли процесс мастером
    is_master = acquire_master_lock();

    // Логируем запуск программы
    log_message("Program started. Master: " + std::to_string(is_master));

    // Запускаем таймер и логирование
    std::thread timer_thread(timer_300ms);
    std::thread logger_thread(log_status_every_second);

    // Если процесс мастер, запускаем управление дочерними процессами
    if (is_master) {
        std::thread child_manager_thread(manage_children);
        child_manager_thread.detach();
    }

    // Обработка пользовательского ввода
    user_input();

    // Завершение работы
    timer_thread.detach();
    logger_thread.detach();
    log_file.close();

    return 0;
}
