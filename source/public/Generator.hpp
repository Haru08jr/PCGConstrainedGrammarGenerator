#pragma once
#include <map>
#include <utility>
#include <vector>

#include "automaton/Edge.hpp"
#include "automaton/NFA.hpp"

struct GenerationConstraint {
    char symbol;
    float position;

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

/*
struct GenerationResultWithRange : GenerationResult {
    std::map<char, SizeRange> symbolSizes;

    GenerationResultWithRange(State initialState, const std::map<char, SizeRange>& initialSymbolSizes)
        : GenerationResult(initialState), symbolSizes(initialSymbolSizes) {
    }

    [[nodiscard]] float GetCurrentMinLength() const {
        float length = 0;
        for (auto c : currentString) {
            length += symbolSizes.at(c).minSize;
        }
        return length;
    }

    [[nodiscard]] float GetCurrentMaxLength() const {
        float length = 0;
        for (auto c : currentString) {
            length += symbolSizes.at(c).maxSize;
        }
        return length;
    }

    bool operator<(const GenerationResultWithRange& other) const {
        if (epsilons == other.epsilons) {
            return GetCurrentMinLength() < other.GetCurrentMinLength();
        }
        return epsilons > other.epsilons;
    }
};*/

class Generator {
public:
    static GenerationResult generate(const std::map<char, float>& symbolSizes, float maxLength, const NFA& nfa, std::vector<GenerationConstraint> constraints);

    //static GenerationResultWithRange generate(const map<char, SizeRange>& symbolSizes, float maxLength, const NFA& nfa, vector<GenerationConstraint> constraints);
};
