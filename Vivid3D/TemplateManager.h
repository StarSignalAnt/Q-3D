#pragma once
#ifndef TEMPLATEMANAGER_H
#define TEMPLATEMANAGER_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

class TemplateManager {
public:
    TemplateManager() = default;

    // Load template from a file
    bool LoadTemplate(const std::string& filePath);

    // Save template to a file
    bool SaveTemplate(const std::string& filePath) const;

    // Replace occurrences of searchText with replaceText in the template
    void SwitchText(const std::string& searchText, const std::string& replaceText);

private:
    std::vector<std::string> templateLines;
};

#endif // TEMPLATEMANAGER_H