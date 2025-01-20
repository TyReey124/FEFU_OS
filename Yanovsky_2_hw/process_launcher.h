
#ifndef PROCESS_LAUNCHER_H
#define PROCESS_LAUNCHER_H

#include <string>
#include <optional>

namespace process_launcher {

std::optional<int> run(const std::string& command, bool wait);

}

#endif
