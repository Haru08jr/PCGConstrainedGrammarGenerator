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
    const auto& eNFA = compiler.getConstructedNFA();

    const NFAGraph eGraph(eNFA);
    eGraph.printGraph("eGraph");

    NonEpsilonNFA neNFA(eNFA);
    const NFAGraph neGraph(neNFA);
    neGraph.printGraph("neGraph");

    std::map<std::string, GrammarModule> modules;
    modules.emplace("w", GrammarModule{"w", 2.f});
    modules.emplace("c", GrammarModule{"c", 0.5f});
    modules.emplace("d", GrammarModule{"d", 1.f});

    std::vector<GenerationConstraint> constraints;
    constraints.emplace_back("d", 2.f);

    Generator generator(modules, 10.6f, eNFA, constraints);
    std::cout << generator.getGenerationResult().getGeneratedString() << std::flush;

    return 0;
}
