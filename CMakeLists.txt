cmake_minimum_required(VERSION 4.1)
project(PCGGrammarsWithConstraints)

set(CMAKE_CXX_STANDARD 20)

add_executable(PCGGrammarsWithConstraints main.cpp
        source/private/RegexPreprocessor.cpp
        source/public/RegexPreprocessor.hpp
        libraries/ThompsonsConstruction/re_compiler.hpp
        source/private/Generator.cpp
        source/public/Generator.hpp)
