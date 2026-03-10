#pragma once

#include <map>
#include <queue>
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

struct GrammarModule
{    
    std::string symbol;
    float size;
    bool spawnOnlyWithConstraint = false;
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

    /** Shorter results with less epsilons are larger (and therefore prioritized in a queue) */
    bool operator<(const GenerationResult& other) const {
        if (epsilons != other.epsilons)
            return epsilons > other.epsilons;

        return currentLength > other.currentLength;
    }

    [[nodiscard]] bool isValid() const {
        return currentState != -1;
    }
};

enum class GenerationErrorType {
    NoError,
    UnknownLiteral,
    ConstraintsNotSatisfiable
};

struct GenerationException : std::exception {
    explicit GenerationException(const GenerationErrorType errorType) : errorType(errorType) {}
    GenerationErrorType errorType;
};

class Generator {
public:
    Generator(const std::map<std::string, GrammarModule>& modules, float maxLength, const NFA& nfa, std::vector<GenerationConstraint> constraints);
    
    const GenerationResult& getGenerationResult() const;
    bool wasGenerationSuccessful() const;
    GenerationErrorType getErrorInfo() const;
    
private:
    GenerationResult result;
    GenerationErrorType errorType;
    
    static GenerationResult generate(const std::map<std::string, GrammarModule>& modules, float maxLength, const NFA& nfa, std::vector<GenerationConstraint> constraints);

    static void applyTransitionAndAddToQueue(std::priority_queue<GenerationResult>& queue, const GenerationResult& previousResult, const Edge& transition, const std::map<std::string, GrammarModule>& modules, std::vector<GenerationConstraint>
                                             constraints);
};
