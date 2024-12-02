#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <vector>


typedef enum {
    C_EXIT,     // exit
    C_ECHO,     // echo
    C_TYPE,     // type
    C_ERROR,    // error
    C_EXEC,     // other
} ShellContext;

typedef struct {
    bool error;
    std::vector<std::string> exec_list;
    std::vector<std::string> loc_list;
} PATHMapping;


std::vector<std::string> build_in_functions = { "echo", "type", "exit" };


ShellContext getShellContext(std::string input);
PATHMapping buildPATHMap(std::string path);
bool handleCommonContext(std::string input, ShellContext context);
bool handleTypeContext(std::string input, ShellContext context, PATHMapping pm);
std::string trimString(size_t start, std::string input);
std::vector<std::string> split(const std::string& str, char delimiter);
void executeCommand(std::string input, ShellContext context, PATHMapping file_map);


int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    const char* path_env = std::getenv("PATH");
    PATHMapping file_map = buildPATHMap(path_env);

    while(true) {
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);

        if (input.empty()) continue;

        // Get context to continue
        ShellContext context = getShellContext(input);
        bool wasCommonContextHandled = handleCommonContext(input, context);

        if (wasCommonContextHandled) {
            continue;
        }

        bool wasTypeContextHandled = handleTypeContext(input, context, file_map);
        if (wasTypeContextHandled) {
            continue;
        }

        executeCommand(input, context, file_map);

        std::cout << "unhandled error" << std::endl;
    }

    // const std::string path(path_env);
    // std::vector<std::string> paths = split_path(path);

    // // Get all file names from PATH directories
    // std::vector<FileAndCommand> file_list = build_file_list(paths);

    // while (true) {
    //     std::cout << "$ ";
    //     std::string input;
    //     std::getline(std::cin, input);

    //     if (input.empty()) {
    //         continue; // Ignore empty input
    //     }

    //     CommandType rc = identify_first_command(input);

    //     if (rc == C_TYPE) {
    //         std::string command_descriptor = input.substr(5); // Extract argument
    //         if (command_descriptor == "echo") {
    //             std::cout << "echo is a shell builtin" << std::endl;
    //         } else if (command_descriptor == "exit") {
    //             std::cout << "exit is a shell builtin" << std::endl;
    //         } else if (command_descriptor == "type") {
    //             std::cout << "type is a shell builtin" << std::endl;
    //         } else {
    //             auto it = std::find_if(file_list.begin(), file_list.end(),
    //                                    [&command_descriptor](const FileAndCommand& entry) {
    //                                    return entry.file == command_descriptor;
    //                                    });

    //             if (it != file_list.end()) {
    //                 std::cout << command_descriptor << " is " << it->path << "/" << it->file << std::endl;
    //             } else {
    //                 std::cout << command_descriptor << ": not found" << std::endl;
    //             }
    //         }
    //     } else if (rc == C_ECHO) {
    //         std::cout << input.substr(5) << std::endl; // Output everything after "echo "
    //     } else if (rc == C_EXIT) {
    //         break;
    //     } else if (rc == C_ERROR) {
    //         std::cout << input << ": command not found" << std::endl;
    //     }
    // }

    return EXIT_SUCCESS;
}


void executeCommand(std::string input, ShellContext context, PATHMapping file_map) {

}


PATHMapping buildPATHMap(std::string path) {
    PATHMapping pm;

    if (path.empty()) {
        pm.error = true;
        return pm;
    }

    std::vector<std::string> exec_locations = split(path, ':');

    for (auto exec_folder : exec_locations) {
        for (auto entry : std::filesystem::directory_iterator(exec_folder)) {
            if (entry.is_regular_file()) {
                pm.loc_list.push_back(exec_folder);
                pm.exec_list.push_back(entry.path().filename().string());
            }
        }
    }

    pm.error = true;
    return pm;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::string current;

    for (char ch : str) {
        if (ch == delimiter) {
            result.push_back(current);
            current.clear();
        } else {
            current += ch;
        }
    }

    if (!current.empty()) {
        result.push_back(current);
    }

    return result;
}

bool handleTypeContext(std::string input, ShellContext context, PATHMapping pm) {
    if (context != C_TYPE) return false;

    std::string context_function = trimString(5, input);

    if (std::find(
        build_in_functions.begin(),
        build_in_functions.end(),
        context_function) != build_in_functions.end()
    ) {
        std::cout << context_function << " is a shell builtin" << std::endl;
        return true;
    }

    auto it = std::find(pm.exec_list.begin(), pm.exec_list.end(), context_function);
    if (it != pm.exec_list.end()) {
        size_t index = std::distance(pm.exec_list.begin(), it);
        std::cout
            << context_function
            << " is "
            << pm.loc_list[index]
            << "/"
            << context_function
            << std::endl;

        return true;
    }

    std::cout << context_function << ": not found" << std::endl;
    return true;
}


ShellContext getShellContext(std::string input) {
    if (input.rfind("exit 0", 0) == 0) return C_EXIT;
    if (input.rfind("echo", 0) == 0) return C_ECHO;
    if (input.rfind("type", 0) == 0) return C_TYPE;

    return C_ERROR;
}


// Return fase if unhandled
bool handleCommonContext(std::string input, ShellContext context) {
    if (context == C_ECHO) {
        std::cout << trimString(5, input) << std::endl;
        return true;
    }

    if (context == C_EXIT) {
        exit(0);
    }

    if (context == C_ERROR) {
        std::cout << input << ": command not found" << std::endl;
        return true;
    }

    return false;
}


std::string trimString(size_t start, std::string input) {
    if (start >= input.length()) return "";

    std::string buffer;
    for (size_t idx = start; idx < input.length(); idx++) {
        buffer += input[idx];
    }

    return buffer;
}
