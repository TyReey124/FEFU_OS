
#include "process_launcher.h"
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace process_launcher {

std::optional<int> run(const std::string& command, bool wait) {
    #ifdef _WIN32
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, const_cast<char*>(command.c_str()), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        return std::nullopt;
    }
    if (wait) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD exit_code;
        if (GetExitCodeProcess(pi.hProcess, &exit_code)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return static_cast<int>(exit_code);
        }
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return std::nullopt;
    #else
    pid_t pid = fork();
    if (pid == -1) {
        return std::nullopt;
    } else if (pid == 0) {
        execl("/bin/sh", "sh", "-c", command.c_str(), (char*)nullptr);
        _exit(EXIT_FAILURE);
    } else {
        if (wait) {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            }
        }
        return std::nullopt;
    }
    #endif
}

}
