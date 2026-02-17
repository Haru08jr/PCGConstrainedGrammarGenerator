#pragma once
#include <string>

#include "RegularExpression.hpp"

enum RegexOperators : char{
    StartGroup = '[',
    EndGroup = ']',
    And = ',',
    Or = '|',
    Kleene = '*',
    Plus = '+',
    Optional = '?'
};

class RegexParser {
public:
    explicit RegexParser(std::string regexString);

    [[nodiscard]] std::shared_ptr<RegularExpression> getParsedRegex() const;

private:
    std::string regexString;
    int parseIndex;
    std::shared_ptr<RegularExpression> parsedRegexTree;

    bool consumeSpecifiedChar(char c);
    char consumeNextChar();
    [[nodiscard]] char peekNextChar() const;

    static bool isOperator(char c);

    std::unique_ptr<RegularExpression> parseValue();
    std::unique_ptr<RegularExpression> parseLiteral();
    std::unique_ptr<RegularExpression> parseRepetition();
    std::unique_ptr<RegularExpression> parseConcatenation();
    std::unique_ptr<RegularExpression> parseAlternative();
    std::unique_ptr<RegularExpression> parseGroup();
};
