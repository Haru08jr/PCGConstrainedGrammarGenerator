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
    return regexString[parseIndex++];
}

char RegexParser::peekNextChar() const {
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
    while (!isOperator(peekNextChar())) {
        literalString += consumeNextChar();
    }
    return std::make_unique<LiteralRegex>(literalString);
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
    consumeSpecifiedChar(StartGroup);
    auto regex = parseAlternative();
    consumeSpecifiedChar(EndGroup);

    return regex;
}

RegexParser::RegexParser(std::string regexString): regexString(std::move(regexString)), parseIndex(0) {
    parsedRegexTree = parseGroup();
}

std::shared_ptr<RegularExpression> RegexParser::getParsedRegex() const {
    return parsedRegexTree;
}
