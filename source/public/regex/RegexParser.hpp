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

/**
 * Used to construct a RegularExpression tree from a string.
 * Parsers are string-specific.
 */
class RegexParser {
public:
    /** Initializes the parser and constructs the regex tree. */
    explicit RegexParser(std::string string);

    /** Access the constructed regex tree. */
    [[nodiscard]] std::shared_ptr<RegularExpression> getParsedRegex() const;

private:
    std::string regexString;
    int parseIndex;
    std::shared_ptr<RegularExpression> parsedRegexTree;

    /** If the next character in the string is c, returns true and advances the index. Else returns false. */
    bool consumeSpecifiedChar(char c);
    /** Returns the next character in the string and advances the index. */
    char consumeNextChar();
    /** Returns the next character in the string. */
    [[nodiscard]] char peekNextChar() const;

    /** Returns true if c is one of the characters specified in RegexOperators. */
    static bool isOperator(char c);

    // parsing functions

    std::unique_ptr<RegularExpression> parseAlternative();
    std::unique_ptr<RegularExpression> parseConcatenation();
    std::unique_ptr<RegularExpression> parseRepetition();
    std::unique_ptr<RegularExpression> parseValue();
    std::unique_ptr<RegularExpression> parseGroup();
    /** Construct a RegularExpression from the literal coming next in the string. */
    std::unique_ptr<RegularExpression> parseLiteral();

    int parseNumber();
};
