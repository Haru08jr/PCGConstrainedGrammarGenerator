#pragma once
#include <string>
#include <utility>

enum RegexSymbol : char {
    Character,/*
    LParenthesis = '[',
    RParenthesis = ']',
    Repeat = '*',
    RepeatAtLeastOnce = '+',
    Option = '?',
    Concatenation = ',',
    Or = '|'*/
};

struct RegexToken {
    RegexSymbol symbol;
    std::string literal;

    explicit RegexToken(const RegexSymbol symbol) : symbol(symbol){}
    explicit RegexToken(std::string  literal) : symbol(Character), literal(std::move(literal)){}

    [[nodiscard]] std::string GetString() const {
        if (symbol == Character)
            return literal;

        return {1, symbol};
    }

    static bool isSpecialSymbol(char c) {
        return false;//c == LParenthesis || c == RParenthesis || c == Repeat || c == RepeatAtLeastOnce || c == Option || c == Concatenation || c == Or;
    }

    static void parseRegexString(const std::string& regex, std::vector<RegexToken>& outTokens) {
        std::string currentLiteral;
        for (auto c: regex) {
            if (isSpecialSymbol(c)) {
                outTokens.emplace_back(currentLiteral);
                currentLiteral.clear();
                outTokens.emplace_back(static_cast<RegexSymbol>(c));
            }
            else {
                currentLiteral += c;
            }
        }
    }
};