#include "../public/RegexPreprocessor.hpp"

#include <iostream>
#include <ostream>
/*
Regex::Regex(const std::string& regexString) : originalString(regexString) {
    std::string current;

    for (const auto symbol: regexString) {
        if (symbol == ' ' || symbol == ',') {
            if (!current.empty()) {
                symbols.push_back(current);
                current.clear();
            }
        } else if (symbol == '(' || symbol == ')' || symbol == '*' || symbol == '|' || symbol == '+' || symbol == '?') {
            if (!current.empty()) {
                symbols.push_back(current);
                current.clear();
            }
            symbols.push_back({symbol});
        } else {
            current += symbol;
        }
    }
}*/

/*
Regex::Regex(const std::string& regexString) : originalString(regexString) {
    std::string current;

    for (const auto symbol: regexString) {
        if (symbol == ' ' || symbol == ',') {
            if (!current.empty()) {
                symbols.emplace_back(current);
                current.clear();
            }
        } else if (symbol == '(' || symbol == ')' || symbol == '*' || symbol == '|' || symbol == '+' || symbol == '?') {
            if (!current.empty()) {
                symbols.emplace_back(current);
                current.clear();
            }
            symbols.emplace_back(symbol);
        } else {
            current += symbol;
        }
    }
}*/

/*
void Regex::printSymbols() const {
    for (const auto& symbol: symbols) {
        std::cout << symbol << "\n";
    }
    std::cout << std::flush;
}*/

RegexPreprocessor::RegexPreprocessor(const std::vector<std::string>& literals) {
    if (literals.size() > 26) {
        std::cout << "Too many literals! Upper limit is 26." << std::endl;
    }

    char charLiteral = 'a';
    for (const auto& literal: literals) {
        auto literalString = std::string(1, charLiteral);
        symbolMapping.insert({charLiteral, literal});

        ++charLiteral;
    }
}
/*
RegexPreprocessor::RegexPreprocessor(const std::string& string, const std::vector<std::string>& literals) : regexString(string) {
    char newLiteral = 'a';
    for (const auto& literal: literals) {
        auto literalString = std::string(1, newLiteral);

        if (findAndReplaceAll(regexString, literal, literalString)) {
            symbolMapping.insert({newLiteral, literal});
            ++newLiteral;
        }
    }
}*/

std::string RegexPreprocessor::shorten(const std::string& string) {
    auto result = string;
    for (const auto& [singleChar, originalLiteral]: symbolMapping) {
        auto charAsString = std::string(1, singleChar);
        findAndReplaceAll(result, originalLiteral, charAsString);
    }
    return result;
}

std::string RegexPreprocessor::translateBack(const std::string& string) const {
    auto result = string;
    for (const auto& [singleChar, originalLiteral]: symbolMapping) {
        auto charAsString = std::string(1, singleChar);
        findAndReplaceAll(result, charAsString, originalLiteral);
    }
    return result;
}

/*
void RegexPreprocessor::printRegexString() const {
    std::cout << regexString << std::flush;
}

std::string RegexPreprocessor::getRegexString() const {
    return regexString;
}*/

bool RegexPreprocessor::findAndReplaceAll(std::string& string, const std::string& from, const std::string& to) {
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

bool RegexPreprocessor::findAndReplaceFirst(std::string& string, const std::string& from, const std::string& to) {
    auto start = string.find(from);
    if (start != std::string::npos) {
        string.replace(start, from.length(), to);
        return true;
    }
    return false;
}