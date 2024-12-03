#include "util/util.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <vector>


void handleExitCommand(std::vector<std::string> input);
void handleEchoCommand(std::vector<std::string> input);
void handleTypeCommand(
    std::vector<std::string> input,
    std::vector<FileLocMap> flm
);
void handleExecutableCommand(
    std::vector<std::string> input,
    std::vector<FileLocMap> flm
);


int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    const std::string path_env = std::getenv("PATH");
    std::vector<std::string> PATHLocations =
        splitStringOnDelimiter(path_env, ":");

    std::vector<FileLocMap> flm = mapFileLocations(PATHLocations);

    while(true) {
        std::cout << "$ ";
        std::string user_input;
        std::getline(std::cin, user_input);

        const std::vector<std::string> input =
            splitStringOnDelimiter(user_input, " ");

        if (input.size() == 0 || input[0] == "") {
            continue;
        }

        const CommandIdentifier ci = identifyCommand(input, flm);

        if (ci == C_ECHO) {
            handleEchoCommand(input);
            continue;
        }

        if (ci == C_EXIT) {
            handleExitCommand(input);
            continue;
        }

        if (ci == C_TYPE) {
            handleTypeCommand(input, flm);
            continue;
        }

        if (ci == C_EXECUTABLE) {
            handleExecutableCommand(input, flm);
            continue;
        }

        if (ci == C_ILLEGAL) {
            std::cout << input[0] << ": command not found" << std::endl;
            continue;
        }

        // for (auto i : input) {
        //     std::cout << i << std::endl;
        // }
    }

    return EXIT_SUCCESS;
}

void handleExecutableCommand(std::vector<std::string> input, std::vector<FileLocMap> flm) {
    std::string path = getFLMExecutablePath(input[0], flm);

    std::string buffer;
    for (size_t idx = 1; idx < input.size(); idx++) {
        buffer += " " + input[idx];
    }

    std::string command = path + "/" + input[0] + buffer;
    const char *command_ptr = command.c_str();
    system(command_ptr);
}

void handleTypeCommand(std::vector<std::string> input, std::vector<FileLocMap> flm) {
    if (input[1] == "exit" || input[1] == "type" || input[1] == "echo") {
        std::cout << input[1] << ": is a shell builtin" << std::endl;
        return;
    }

    if (doesFLMContainsExecutable(input[1], flm)) {
        std::cout
            << input[1]
            << " is "
            << getFLMExecutablePath(input[1], flm)
            << "/"
            << input[1]
            << std::endl;

        return;
    }

    std::cout << input[1] << ": not found" << std::endl;
}

void handleEchoCommand(std::vector<std::string> input) {
    std::string buffer;
    for (size_t idx = 1; idx < input.size(); idx++) {
        if (idx == 1) {
            buffer += input[idx];
            continue;
        }
        buffer += " " + input[idx];
    }

    std::cout << buffer << std::endl;
}

void handleExitCommand(std::vector<std::string> input) {
    if (input[1] == "0") {
        exit(0);
    }

    std::cout << input[0] << ": command not found" << std::endl;
}
