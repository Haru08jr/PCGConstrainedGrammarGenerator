#pragma once

#include <regex>
#include <utility>

enum RegexType {
    Literal = 1,
    Repeat  = 1 << 1,
    RepeatAtLeastOnce = 1 << 2,
    RepeatNTimes = 1 << 3,
    Option = 1 << 4,
    Concatenation = 1 << 5,
    Alternative = 1 << 6,

    Unary = Repeat | RepeatAtLeastOnce | Option,
    Binary = Concatenation | Alternative,

    Invalid = 0
};

class RegularExpression {
public:
    RegexType type;

    explicit RegularExpression(RegexType type) : type(type) {}
    virtual ~RegularExpression() = default;

    [[nodiscard]] bool isValid() const {return type == Invalid;}
};

class LiteralRegex : public RegularExpression {
public:
    std::string literalString;

    explicit LiteralRegex(std::string literalString)
        : RegularExpression(Literal), literalString(std::move(literalString)) {}
};

class UnaryRegex : public RegularExpression {
public:
    std::shared_ptr<RegularExpression> subRegex;

    UnaryRegex(RegexType type, std::shared_ptr<RegularExpression> subRegex)
        : RegularExpression(type & Unary ? type : Invalid), subRegex(std::move(subRegex)) {}
};

class BinaryRegex : public RegularExpression {
public:
    std::shared_ptr<RegularExpression> firstSubRegex;
    std::shared_ptr<RegularExpression> secondSubRegex;

    BinaryRegex(RegexType type, std::shared_ptr<RegularExpression> firstSubRegex, std::shared_ptr<RegularExpression> secondSubRegex)
        : RegularExpression(type & Binary ? type : Invalid), firstSubRegex(std::move(firstSubRegex)), secondSubRegex(std::move(secondSubRegex)) {}
};

class UnaryRegexWithParam : public RegularExpression {
public:
    std::shared_ptr<RegularExpression> subRegex;
    int parameter;

    UnaryRegexWithParam(std::shared_ptr<RegularExpression> subRegex, int parameter)
        : RegularExpression(RepeatNTimes), subRegex(std::move(subRegex)), parameter(parameter) {}
};