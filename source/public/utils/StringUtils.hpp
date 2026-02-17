#pragma once
#include <string>

class StringUtils {
public:
    static bool findAndReplaceAll(std::string& string, const std::string& from, const std::string& to);
    static bool findAndReplaceFirst(std::string& string, const std::string& from, const std::string& to);
};
