#ifndef UTIL_H
#define UTIL_H


#include <string>
#include <vector>


typedef enum {
    C_ECHO,
    C_EXIT,
    C_TYPE,
    C_EXECUTABLE,
    C_ILLEGAL,
} CommandIdentifier;


struct FileLocMap {
    std::string executable;
    std::string location;
};


std::vector<std::string> splitStringOnDelimiter(
    const std::string& input,
    const std::string& delimiter
);

CommandIdentifier identifyCommand(
    std::vector<std::string> input,
    std::vector<FileLocMap> path_locations
);

std::vector<FileLocMap> mapFileLocations(
    std::vector<std::string> path_locations
);

bool doesFLMContainsExecutable(
    std::string exec_name,
    std::vector<FileLocMap> file_map
);

std::string getFLMExecutablePath(
    std::string exec_name,
    std::vector<FileLocMap> file_map
);


#endif // UTIL_H
