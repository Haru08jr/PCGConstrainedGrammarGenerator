#pragma once
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "automaton/Edge.hpp"
#include "automaton/NFA.hpp"

struct GenerationConstraint {
    std::string symbol;
    float position;
    float halfwith;

    bool operator<(const GenerationConstraint& other) const {
        return position < other.position;
    }
};

struct SizeRange {
    float minSize;
    float maxSize;
};

struct GenerationResult {
    explicit GenerationResult(State initialState) : currentState(initialState) {}

    std::vector<std::string> currentSymbols;
    State currentState;

    int epsilons = 0;
    float currentLength = 0;
    int constraintsMet = 0;

    [[nodiscard]] std::string getGeneratedString() const {
        std::string result;
        for (const auto& symbol : currentSymbols) {
            result += symbol;
            result += ",";
        }
        return  result.substr(0, result.length() - 1);
    }

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
    static GenerationResult generate(const std::map<std::string, float>& symbolSizes, float maxLength, const NFA& nfa, std::vector<GenerationConstraint> constraints);
};
