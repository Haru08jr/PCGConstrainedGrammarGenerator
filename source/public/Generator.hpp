#pragma once
#include <map>

#include "../../libraries/ThompsonsConstruction/re_compiler.hpp"

struct GenerationConstraint {
    char symbol;
    float position;

    bool operator<(const GenerationConstraint& other) const {
        return position < other.position;
    }
};

struct GenerationResult {
    explicit GenerationResult(State initialState) : currentState(initialState) {}

    std::string currentString;
    State currentState;

    int epsilons = 0;
    float currentLength = 0;
    int constraintsMet = 0;

    bool operator<(const GenerationResult& other) const {
        if (epsilons == other.epsilons) {
            return currentLength < other.currentLength;
        }
        return epsilons > other.epsilons;
    }

    [[nodiscard]] bool isValid() const {
        return currentState != -1;
    }
};

class Generator {
public:
    static GenerationResult generate(map<char, float>& symbolSize, float maxLength, NFA& nfa, vector<GenerationConstraint>& constraints);
};
