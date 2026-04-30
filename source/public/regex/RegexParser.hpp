#pragma once
#include <set>
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

enum class RegexErrorType {
    NoError,
    EmptyString,
    MissingLiteral,
    UnknownLiteral,
    InvalidString
};

struct RegexParsingException : std::exception{
    RegexErrorType errorType;

    explicit RegexParsingException(const RegexErrorType errorType) : errorType(errorType) {}
};

/**
 * Used to construct a RegularExpression tree from a string.
 * Parsers are string-specific.
 */
class RegexParser {
public:
    /** Initializes the parser and constructs the regex tree. */
    explicit RegexParser(std::string string, const std::set<std::string>& literals);

    /** Access the constructed regex tree. */
    [[nodiscard]] std::shared_ptr<RegularExpression> getParsedRegex() const;

    [[nodiscard]] bool wasParsingSuccessful() const;
    [[nodiscard]] RegexErrorType getErrorInfo() const;

private:
    std::string regexString;
    const std::set<std::string> allowedLiterals;
    int parseIndex;
    std::shared_ptr<RegularExpression> parsedRegexTree;

    RegexErrorType parsingError;

    /** If the next character in the string is c, returns true and advances the index. Else returns false. */
    bool consumeSpecifiedChar(char c);
    /** Returns the next character in the string and advances the index. */
    char consumeNextChar();
    /** Returns the next character in the string. */
    [[nodiscard]] char peekNextChar() const;

    /** Returns true if c is one of the characters specified in RegexOperators. */
    static bool isOperator(char c);

    /** Checks that this literal is contained in the set of allowed literals. */
    [[nodiscard]] bool isLiteralAllowed(const std::string& literal) const;

    /*
     * Parsing functions: recursively construct a regex syntax tree.
     * Operators bind in the order Group > Repetition > Concatenation > Alternative, therefore the functions should be called in reverse order.
     */

    /** Parse a chain of alternative expressions and return the resulting regex tree. */
    std::unique_ptr<RegularExpression> parseAlternative();
    /** Parse a chain of concatenated expressions and return the resulting regex tree. */
    std::unique_ptr<RegularExpression> parseConcatenation();
    /** Construct a unary RegularExpression repeating the subexpression. */
    std::unique_ptr<RegularExpression> parseRepetition();
    /** If a group is present, restart parsing the subexpression. Else parse the subexpression as a literal. */
    std::unique_ptr<RegularExpression> parseGroup();

    /** Construct a RegularExpression from the literal coming next in the string. */
    std::unique_ptr<RegularExpression> parseLiteral();
    /** Parse the number coming next in the string. */
    int parseNumber();
};
