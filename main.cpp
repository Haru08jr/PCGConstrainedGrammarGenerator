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
    modules.emplace("a", GrammarModule{"a", 0.5f});
    modules.emplace("b", GrammarModule{"b", 0.25f});

    //const auto parseTimeStart = std::chrono::high_resolution_clock::now();
    const RegexParser parser("[a|b]*", {"a", "b"});
    const auto regex = parser.getParsedRegex();
    //const auto parseTimeEnd = std::chrono::high_resolution_clock::now();
    //std::cout << (parseTimeEnd - parseTimeStart) << std::endl;

    //const auto compileTimeStart = std::chrono::high_resolution_clock::now();
    const NFACompiler compiler(regex);
    const auto& eNFA = compiler.getConstructedNFA();
    //const auto compileTimeEnd = std::chrono::high_resolution_clock::now();
    //std::cout << (compileTimeEnd - compileTimeStart) << std::endl;

    //const NFAGraph eGraph(eNFA);
    //eGraph.printGraph("eGraph");

    std::vector<GenerationConstraint> constraints;
    /*
    constraints.emplace_back("b", 2.5f);
    constraints.emplace_back("b", 5.f);
    constraints.emplace_back("b", 7.5f);*/


    for (int i = 0; i < 10; i++) {
        const auto generateTimeStart = std::chrono::high_resolution_clock::now();
        Generator generator(modules, 5.1f, eNFA, constraints);
        const auto generateTimeEnd = std::chrono::high_resolution_clock::now();
        std::cout << (generateTimeEnd - generateTimeStart) << std::endl;
        if (i == 9)
            std::cout << generator.getGenerationResult().getGeneratedString() << std::endl << std::flush;
    }
    return 0;
}
