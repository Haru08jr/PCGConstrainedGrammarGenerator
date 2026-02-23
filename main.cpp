#pragma once

#include <iostream>

#include <graaflib/graph.h>
#include <Generator.hpp>
#include <automaton/NFACompiler.hpp>
#include <regex/RegexParser.hpp>

#include "utils/GraphUtils.hpp"

int main() {
    const RegexParser parser("[w|d]*");
    const auto regex = parser.getParsedRegex();

    const NFACompiler compiler(regex);
    const auto nfa = compiler.getConstructedNFA();

    const NFAGraph graph(nfa);
    graph.printGraph();

    std::map<std::string, float> symbolSize;
    symbolSize.emplace("w", 2.f);
    symbolSize.emplace("c", 0.5f);
    symbolSize.emplace("d", 1.f);

    std::vector<GenerationConstraint> constraints;
    constraints.emplace_back("d", 2.5f);

    const auto result = Generator::generate(symbolSize, 7.1f, nfa, constraints);
    std::cout << result.getGeneratedString() << std::flush;

    return 0;
}
