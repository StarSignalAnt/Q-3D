#pragma once
#include <string>
class FileHelp
{
public:

    static std::string GetFileName(const std::string& filePath) {
        // Find the position of the last path separator
        size_t lastSlash = filePath.find_last_of("/\\");
        std::string fileName;

        if (lastSlash == std::string::npos) {
            fileName = filePath; // No path separator found
        }
        else {
            fileName = filePath.substr(lastSlash + 1);
        }

        // Find the position of the last dot
        size_t lastDot = fileName.find_last_of('.');
        if (lastDot != std::string::npos) {
            fileName = fileName.substr(0, lastDot);
        }

        return fileName;
    }

};

