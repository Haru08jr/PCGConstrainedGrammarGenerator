#pragma once

#include <iostream>
#include "source/public/RegexPreprocessor.hpp"
#include "source/public/Generator.hpp"
#include "source/public/automaton/NFACompiler.hpp"
#include "source/public/regex/RegexParser.hpp"

int main() {
    /*
    std::string regex = "(w|cdc)*";

    NFACompiler compiler;
    NFA nfa = compiler.compile(regex);

    std::map<char, float> symbolSize;
    symbolSize.emplace('w', 2.f);
    symbolSize.emplace('c', 0.5f);
    symbolSize.emplace('d', 1.f);

    std::vector<GenerationConstraint> constraints;
    constraints.emplace_back('d', 3.f);
    //constraints.emplace_back('d', 19.f);

    auto result = Generator::generate(symbolSize, 7.1f, nfa, constraints);
    std::cout << result.currentString << std::flush;
*/
    const RegexParser parser("[a*, b, a*]");
    const auto regex = parser.getParsedRegex();

    const NFACompiler compiler(regex);
    auto nfa = compiler.getConstructedNFA();

    return 0;
}
