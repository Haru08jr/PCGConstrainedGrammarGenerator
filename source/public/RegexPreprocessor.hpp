#pragma once
#include <map>
#include <string>
#include <vector>

/**
 * Preprocess a regex string
 */
class RegexPreprocessor {
public:
    RegexPreprocessor(const std::vector<std::string>& literals);

    /**
     * Shortens the regex by replacing all literal strings with a single character.
     * To be used if the literals are longer than one single character.
     * Note: only works for up to 26 literals TODO can this be done better?
     */
    //RegexPreprocessor(const std::string& string, const std::vector<std::string>& literals);

    [[nodiscard]] std::string shorten(const std::string& string);
    [[nodiscard]] std::string translateBack(const std::string& string) const;

    //void printRegexString() const;
    //[[nodiscard]] std::string getRegexString() const;

private:
    //std::string regexString;
    std::map<char, std::string> symbolMapping;

    static bool findAndReplaceAll(std::string& string, const std::string& from, const std::string& to);
    static bool findAndReplaceFirst(std::string& string, const std::string& from, const std::string& to);
};
