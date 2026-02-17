//
// Created by jreut on 10.02.2026.
//

#include "../../public/utils/StringUtils.hpp"

bool StringUtils::findAndReplaceAll(std::string& string, const std::string& from, const std::string& to) {
    if (string.empty() || from.empty()) {
        return false;
    }

    bool replaced = false;
    size_t position = 0;
    while ((position = string.find(from, position)) != std::string::npos) {
        string.replace(position, from.length(), to);
        position += to.length();
        replaced = true;
    }

    return replaced;
}

bool StringUtils::findAndReplaceFirst(std::string& string, const std::string& from, const std::string& to) {
    if (string.empty() || from.empty()) {
        return false;
    }

    auto start = string.find(from);
    if (start != std::string::npos) {
        string.replace(start, from.length(), to);
        return true;
    }
    return false;
}
