#include "TemplateManager.h"

bool TemplateManager::LoadTemplate(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return false;
    }

    templateLines.clear();
    std::string line;
    while (std::getline(file, line)) {
        templateLines.push_back(line);
    }

    file.close();
    return true;
}

bool TemplateManager::SaveTemplate(const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return false;
    }

    for (const auto& line : templateLines) {
        file << line << "\n";
    }

    file.close();
    return true;
}

void TemplateManager::SwitchText(const std::string& searchText, const std::string& replaceText) {
    for (auto& line : templateLines) {
        size_t pos = 0;
        while ((pos = line.find(searchText, pos)) != std::string::npos) {
            line.replace(pos, searchText.length(), replaceText);
            pos += replaceText.length();
        }
    }
}