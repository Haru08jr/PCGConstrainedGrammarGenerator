
#include "regex/RegexParser.hpp"

#include <utility>

#include "utils/StringUtils.hpp"

bool RegexParser::consumeSpecifiedChar(char c) {
    if (peekNextChar() == c) {
        ++_parseIndex;
        return true;
    }
    return false;
}

char RegexParser::consumeNextChar() {
    if (_parseIndex >= _regexString.size())
        return -1;
    return _regexString[_parseIndex++];
}

char RegexParser::peekNextChar() const {
    if (_parseIndex >= _regexString.size())
        return -1;

    return _regexString[_parseIndex];
}

bool RegexParser::isOperator(char c) {
    switch (c) {
        case StartGroup:
        case EndGroup:
        case And:
        case StartOr:
        case EndOr:
        case Kleene:
        case Plus:
        case Optional: return true;
        default: return false;
    }
}

bool RegexParser::isLiteralAllowed(const std::string& literal) const {
    return _allowedLiterals.contains(literal);
}

std::unique_ptr<RegularExpression> RegexParser::parseLiteral() {
    std::string literalString;
    while (!isOperator(peekNextChar()) && peekNextChar() != -1) {
        literalString += consumeNextChar();
    }

    if (literalString.empty())
        throw RegexParsingException(RegexErrorType::MissingLiteral);
    if (!isLiteralAllowed(literalString))
        throw RegexParsingException(RegexErrorType::UnknownLiteral);

    return std::make_unique<LiteralRegex>(literalString);
}

int RegexParser::parseNumber() {
    int number = 0;
    while (isdigit(peekNextChar())) {
        number *= 10;
        number += consumeNextChar() - '0';
    }
    return number;
}

std::unique_ptr<RegularExpression> RegexParser::parseConcatenation() {
    auto regex = parseRepetition();
    while (consumeSpecifiedChar(And)) {
        regex = std::make_unique<BinaryRegex>(Concatenation, std::move(regex), parseRepetition());
    }
    return regex;
}

std::unique_ptr<RegularExpression> RegexParser::parseRepetition() {
    auto regex = parseGroup();

    if (consumeSpecifiedChar(Optional))
        regex = std::make_unique<UnaryRegex>(Option, std::move(regex));
    else if (consumeSpecifiedChar(Kleene))
        regex = std::make_unique<UnaryRegex>(Repeat, std::move(regex));
    else if (consumeSpecifiedChar(Plus))
        regex = std::make_unique<UnaryRegex>(RepeatAtLeastOnce, std::move(regex));

    return regex;
}

std::unique_ptr<RegularExpression> RegexParser::parseGroup() {
    // go into parse group (repeating the recursive cycle) only if a StartGroup symbol is present
    if (consumeSpecifiedChar(StartGroup)) {
        // parse the subexpression, starting at concatenation
        auto regex = parseConcatenation();

        // error if group end is missing
        if (!consumeSpecifiedChar(EndGroup))
            throw RegexParsingException(RegexErrorType::InvalidString);

        // if a number can be found after the group: repeat the group that many times
        if (isdigit(peekNextChar()))
            regex = std::make_unique<UnaryRegexWithParam>(std::move(regex), parseNumber());

        return regex;
    }

    // go into parse alternative only if a StartOr symbol is present
    if (consumeSpecifiedChar(StartOr)) {
        // parse the subexpression, starting at repetition
        // (concatenation in an alternative is only possible when it is contained in a group, because the comma operator is shared)
        auto regex = parseRepetition();
        while (consumeSpecifiedChar(And)) {
            regex = std::make_unique<BinaryRegex>(Alternative, std::move(regex), parseRepetition());
        }

        // error if end is missing
        if (!consumeSpecifiedChar(EndOr))
            throw RegexParsingException(RegexErrorType::InvalidString);

        return regex;
    }

    // else, you have arrived at the lowest level
    return parseLiteral();
}

RegexParser::RegexParser(std::string string, const std::set<std::string>& literals)
    : _regexString(std::move(string)), _allowedLiterals(literals), _parseIndex(0), _parsingError(RegexErrorType::NoError) {
    // remove spaces in string
    StringUtils::findAndReplaceAll(_regexString, " ", "");

    try {
        if (_regexString.empty())
            throw RegexParsingException(RegexErrorType::EmptyString);

        // start parsing at concatenation
        _parsedRegexTree = parseConcatenation();

        // if parsing returns before reaching the end of the string, the regex string must be invalid
        if (_parseIndex < _regexString.size()) {
            throw RegexParsingException(RegexErrorType::InvalidString);
        }
    }catch(RegexParsingException& e) {
        _parsingError = e.errorType;
    }
}

std::shared_ptr<RegularExpression> RegexParser::getParsedRegex() const {
    return _parsedRegexTree;
}

bool RegexParser::wasParsingSuccessful() const {
    return _parsingError == RegexErrorType::NoError;
}

RegexErrorType RegexParser::getErrorInfo() const {
    return _parsingError;
}
