cmake_minimum_required(VERSION 4.1)
project(PCGGrammarsWithConstraints)

set(CMAKE_CXX_STANDARD 20)

add_executable(PCGGrammarsWithConstraints main.cpp
        source/private/RegexPreprocessor.cpp
        source/public/RegexPreprocessor.hpp
        source/private/Generator.cpp
        source/public/Generator.hpp
        source/private/utils/StringUtils.cpp
        source/private/automaton/NFA.cpp
        source/public/automaton/NFA.hpp
        source/public/automaton/Edge.hpp
        source/private/automaton/NFACompiler.cpp
        source/public/automaton/NFACompiler.hpp
        source/public/regex/RegularExpression.hpp
        source/private/regex/RegexParser.cpp
        source/public/regex/RegexParser.hpp
)
