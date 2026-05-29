#pragma once

#include <iostream>
#include <chrono>

#include <graaflib/graph.h>
#include <Generator.hpp>
#include <ranges>
#include <automaton/NFACompiler.hpp>
#include <regex/RegexParser.hpp>

#include "utils/GraphUtils.hpp"

int main() {
    std::map<std::string, GrammarModule> modules;
    modules.emplace("a", GrammarModule{"a", 2.f});
    modules.emplace("b", GrammarModule{"b", 1.f});

    const RegexParser parser("{a,b}*", {"a", "b"});
    const auto regex = parser.getParsedRegex();

    const NFACompiler compiler(regex);
    const auto& eNFA = compiler.getConstructedNFA();

    //const NFAGraph eGraph(eNFA);
    //eGraph.printGraph("eGraph");

    std::vector<GenerationConstraint> constraints;

    constraints.emplace_back("b", 25.f);
    //constraints.emplace_back("a", 0.f);

    //for (int i = 0; i < 10; i++) {
        //const auto generateTimeStart = std::chrono::high_resolution_clock::now();
        Generator generator(modules, 25.1f, eNFA, constraints);
        //const auto generateTimeEnd = std::chrono::high_resolution_clock::now();
        //std::cout << (generateTimeEnd - generateTimeStart) << std::endl;
        //if (i == 9)
            std::cout << generator.getGenerationResult().getGeneratedString() << std::endl << std::flush;
    //}
    return 0;
}
