#include "util.h"
#include <cstddef>
#include <regex>
#include <filesystem>


std::vector<FileLocMap> mapFileLocations(
    const std::vector<std::string> path_locations
) {
    std::vector<FileLocMap> fileLocMap;

    for (const std::string& exec_folder : path_locations) {
        if (!std::filesystem::exists(exec_folder)) {
            continue;
        }

        for (const auto& entry : std::filesystem::directory_iterator(exec_folder)) {
            if (entry.is_regular_file()) {
                FileLocMap flm;
                flm.location = exec_folder;
                flm.executable = entry.path().filename().string();
                fileLocMap.push_back(flm);
            }
        }
    }

    return fileLocMap;
}

std::vector<std::string> splitStringOnDelimiter(
    const std::string& input,
    const std::string& delimiter
) {
    std::regex regexz(delimiter);
    std::vector<std::string> list(
        std::sregex_token_iterator(input.begin(), input.end(), regexz, -1),
        std::sregex_token_iterator()
    );
    return list;
}


CommandIdentifier identifyCommand(
    std::vector<std::string> input,
    std::vector<FileLocMap> file_map
) {
    std::string command = input[0];

    if (command == "echo") {
        return C_ECHO;
    }

    if (command == "exit") {
        return C_EXIT;
    }

    if (command == "type") {
        return C_TYPE;
    }

    if (doesFLMContainsExecutable(command, file_map)) {
        return C_EXECUTABLE;
    }

    return C_ILLEGAL;
}

bool doesFLMContainsExecutable(std::string exec_name, std::vector<FileLocMap> file_map) {
    for(size_t idx = 0; idx < file_map.size(); idx++) {
        if (file_map[idx].executable == exec_name) return true;
    }

    return false;
}

std::string getFLMExecutablePath(
    std::string exec_name,
    std::vector<FileLocMap> file_map
) {
    for(size_t idx = 0; idx < file_map.size(); idx++) {
        if (file_map[idx].executable == exec_name) {
            return file_map[idx].location;
        }
    }

    return "";
}
