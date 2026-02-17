//
// Created by jreut on 17.02.2026.
//

#include "RegexParser.hpp"

#include <utility>

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

std::unique_ptr<RegularExpression> RegexParser::parseValue() {
    if (peekNextChar() == StartGroup)
        return parseGroup();
    else
        return parseLiteral();
}

std::unique_ptr<RegularExpression> RegexParser::parseLiteral() {
    std::string literalString;
    while (!isOperator(peekNextChar()) && peekNextChar() != -1) {
        literalString += consumeNextChar();
    }
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

std::unique_ptr<RegularExpression> RegexParser::parseRepetition() {
    auto regex = parseValue();

    if (peekNextChar() == Optional)
        regex = std::make_unique<UnaryRegex>(Option, std::move(regex));
    else if (peekNextChar() == Kleene)
        regex = std::make_unique<UnaryRegex>(Repeat, std::move(regex));
    else if (peekNextChar() == Plus)
        regex = std::make_unique<UnaryRegex>(RepeatAtLeastOnce, std::move(regex));

    return regex;
}

std::unique_ptr<RegularExpression> RegexParser::parseConcatenation() {
    auto regex = parseRepetition();
    while (consumeSpecifiedChar(And)) {
        regex = std::make_unique<BinaryRegex>(Concatenation, std::move(regex), parseRepetition());
    }
    return regex;
}

std::unique_ptr<RegularExpression> RegexParser::parseAlternative() {
    auto regex = parseConcatenation();

    while (consumeSpecifiedChar(Or)) {
        regex = std::make_unique<BinaryRegex>(Alternative, std::move(regex), parseConcatenation());
    }

    return regex;
}

std::unique_ptr<RegularExpression> RegexParser::parseGroup() {
    bool groupStarted = consumeSpecifiedChar(StartGroup);
    auto regex = parseAlternative();
    bool groupEnd = consumeSpecifiedChar(EndGroup);

    // error if group end was found without start
    if (groupStarted != groupEnd)
        throw std::out_of_range("Invalid regex string!");

    // if number after group: repeat group that many times
    if (isdigit(peekNextChar()))
        regex = std::make_unique<UnaryRegexWithParam>(std::move(regex), parseNumber());

    return regex;
}

RegexParser::RegexParser(std::string string): regexString(std::move(string)), parseIndex(0) {
    parsedRegexTree = parseAlternative();

    if (parseIndex < regexString.size()) {
        throw std::out_of_range("Invalid regex string!");
    }
}

std::shared_ptr<RegularExpression> RegexParser::getParsedRegex() const {
    return parsedRegexTree;
}
