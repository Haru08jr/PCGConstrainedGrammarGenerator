#pragma once

#include <iostream>

#include <graaflib/graph.h>
#include <Generator.hpp>
#include <automaton/NFACompiler.hpp>
#include <regex/RegexParser.hpp>

#include "utils/GraphUtils.hpp"

int main() {
    const RegexParser parser("[w|c|d]*");
    const auto regex = parser.getParsedRegex();

    const NFACompiler compiler(regex);
    const auto& nfa = compiler.getConstructedNFA();

    const NFAGraph graph(nfa);
    graph.printGraph();

    std::map<std::string, GrammarModule> modules;
    modules.emplace("w", GrammarModule{"w", 0.1f});
    modules.emplace("c", GrammarModule{"c", 0.5});
    modules.emplace("d", GrammarModule{"d", 1.f});

    std::vector<GenerationConstraint> constraints;
    constraints.emplace_back("d", 2.f);

    Generator generator(modules, 2.6f, nfa, constraints);
    std::cout << generator.getGenerationResult().getGeneratedString() << std::flush;

    return 0;
}
