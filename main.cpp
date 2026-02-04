#pragma once

#include <iostream>
#include "source/public/RegexPreprocessor.hpp"
#include "source/public/Generator.hpp"

int main() {
    //RegexPreprocessor regex("(door|wall)*", {"door","window","wall"});

    //regex.printRegexString();
    //std::cout << regex.translateBack() << std::flush;

    std::string regex = "(w|d)*";

    NFACompiler compiler;
    NFA nfa = compiler.compile(regex);

    std::map<char, float> symbolSize;
    symbolSize.emplace('w', 1.f);
    //symbolSize.emplace('c', 0.5f);
    symbolSize.emplace('d', 2.f);

    std::vector<GenerationConstraint> constraints;
    constraints.emplace_back('d', 3.f);
    //constraints.emplace_back('d', 19.f);

    auto result = Generator::generate(symbolSize, 7.1f, nfa, constraints);

    std::cout << result.currentString << std::flush;
    return 0;
}