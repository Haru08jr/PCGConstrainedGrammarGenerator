//
// Created by jreut on 17.02.2026.
//

#include "../../public/regex/RegexParser.hpp"

#include <utility>

#include "../../public/utils/StringUtils.hpp"

bool RegexParser::consumeSpecifiedChar(char c) {
    if (peekNextChar() == c) {
        ++parseIndex;
        return true;
    }
    return false;
}

char RegexParser::consumeNextChar() {
    if (parseIndex >= regexString.size())
        return -1;
    return regexString[parseIndex++];
}

char RegexParser::peekNextChar() const {
    if (parseIndex >= regexString.size())
        return -1;

    return regexString[parseIndex];
}

bool RegexParser::isOperator(char c) {
    switch (c) {
        case StartGroup:
        case EndGroup:
        case And:
        case Or:
        case Kleene:
        case Plus:
        case Optional: return true;
        default: return false;
    }
}

std::unique_ptr<RegularExpression> RegexParser::parseLiteral() {
    std::string literalString;
    while (!isOperator(peekNextChar()) && peekNextChar() != -1) {
        literalString += consumeNextChar();
    }

    if (literalString.empty())
        throw RegexParsingException("Invalid regex string!");

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

std::unique_ptr<RegularExpression> RegexParser::parseAlternative() {
    auto regex = parseConcatenation();

    while (consumeSpecifiedChar(Or)) {
        regex = std::make_unique<BinaryRegex>(Alternative, std::move(regex), parseConcatenation());
    }

    return regex;
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
        auto regex = parseAlternative();

        // error if group end is missing
        if (!consumeSpecifiedChar(EndGroup))
            throw RegexParsingException("Invalid regex string!");

        // if number after group: repeat group that many times
        if (isdigit(peekNextChar()))
            regex = std::make_unique<UnaryRegexWithParam>(std::move(regex), parseNumber());

        return regex;
    }
    // else, you have arrived at the lowest level
    return parseLiteral();
}

RegexParser::RegexParser(std::string string) : regexString(std::move(string)), parseIndex(0) {
    // remove spaces in string
    StringUtils::findAndReplaceAll(regexString, " ", "");

    try {
        if (regexString.empty())
            throw RegexParsingException("Empty regex string!");

        // start parsing
        parsedRegexTree = parseAlternative();

        // if parsing returns before reaching the end of the string, the regex string must be invalid
        if (parseIndex < regexString.size()) {
            throw RegexParsingException("Invalid regex string!");
        }
    }catch(RegexParsingException& e) {
        errorMessage = e.errorMessage;
    }
}

std::shared_ptr<RegularExpression> RegexParser::getParsedRegex() const {
    return parsedRegexTree;
}

bool RegexParser::wasParsingSuccessful() const {
    return parsedRegexTree != nullptr && parsedRegexTree->isValid();
}

std::string RegexParser::getErrorMessage() const {
    return errorMessage;
}
