#pragma once

#include <iostream>
#include "source/public/Generator.hpp"
#include "source/public/automaton/NFACompiler.hpp"
#include "source/public/regex/RegexParser.hpp"

int main() {
    const RegexParser parser("[w|d]*");
    const auto regex = parser.getParsedRegex();

    const NFACompiler compiler(regex);
    auto nfa = compiler.getConstructedNFA();

    std::map<std::string, float> symbolSize;
    symbolSize.emplace("w", 2.f);
    symbolSize.emplace("c", 0.5f);
    symbolSize.emplace("d", 1.f);

    std::vector<GenerationConstraint> constraints;
    constraints.emplace_back("d", 3.f);

    const auto result = Generator::generate(symbolSize, 7.1f, nfa, constraints);
    std::cout << result.getGeneratedString() << std::flush;

    return 0;
}
