#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>

typedef enum {
    C_EXIT,
    C_ECHO,
    C_TYPE,
    C_ERROR,
} CommandType;

struct FileAndCommand {
    std::string file;
    std::string path;
};

CommandType identify_first_command(const std::string& input);
std::vector<std::string> split_path(const std::string& path);
std::vector<FileAndCommand> build_file_list(const std::vector<std::string>& paths);

int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    const char* path_env = std::getenv("PATH");
    if (!path_env) {
        std::cerr << "Error: PATH environment variable not found." << std::endl;
        return EXIT_FAILURE;
    }

    const std::string path(path_env);
    std::vector<std::string> paths = split_path(path);

    // Get all file names from PATH directories
    std::vector<FileAndCommand> file_list = build_file_list(paths);

    while (true) {
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);

        if (input.empty()) {
            continue; // Ignore empty input
        }

        CommandType rc = identify_first_command(input);

        if (rc == C_TYPE) {
            std::string command_descriptor = input.substr(5); // Extract argument
            if (command_descriptor == "echo") {
                std::cout << "echo is a shell builtin" << std::endl;
            } else if (command_descriptor == "exit") {
                std::cout << "exit is a shell builtin" << std::endl;
            } else if (command_descriptor == "type") {
                std::cout << "type is a shell builtin" << std::endl;
            } else {
                auto it = std::find_if(file_list.begin(), file_list.end(),
                                       [&command_descriptor](const FileAndCommand& entry) {
                                       return entry.file == command_descriptor;
                                       });

                if (it != file_list.end()) {
                    std::cout << command_descriptor << " is " << it->path << "/" << it->file << std::endl;
                } else {
                    std::cout << command_descriptor << ": not found" << std::endl;
                }
            }
        } else if (rc == C_ECHO) {
            std::cout << input.substr(5) << std::endl; // Output everything after "echo "
        } else if (rc == C_EXIT) {
            break;
        } else if (rc == C_ERROR) {
            std::cout << input << ": command not found" << std::endl;
        }
    }

    return EXIT_SUCCESS;
}

CommandType identify_first_command(const std::string& input) {
    if (input == "exit 0") {
        return C_EXIT;
    }

    if (input.rfind("echo ", 0) == 0) {
        return C_ECHO;
    }

    if (input.rfind("type ", 0) == 0) {
        return C_TYPE;
    }

    return C_ERROR;
}

std::vector<std::string> split_path(const std::string& path) {
    std::vector<std::string> result;
    std::stringstream ss(path);
    std::string segment;

    while (std::getline(ss, segment, ':')) {
        result.push_back(segment);
    }

    return result;
}

std::vector<FileAndCommand> build_file_list(const std::vector<std::string>& paths) {
    std::vector<FileAndCommand> file_list;

    for (const auto& dir : paths) {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(dir)) {
                if (entry.is_regular_file()) {
                    FileAndCommand fac;
                    fac.file = entry.path().filename().string(); // Get file name
                    fac.path = dir;
                    file_list.push_back(fac);
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            // Skip inaccessible directories
            std::cerr << "Warning: Could not access directory " << dir << ": " << e.what() << std::endl;
        }
    }

    return file_list;
}
