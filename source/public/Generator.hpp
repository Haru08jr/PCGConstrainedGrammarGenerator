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
    explicit GenerationResult(const State initialState) : currentState(initialState) {}

    std::vector<std::string> currentSymbols;
    State currentState;
    float currentLength = 0;
    int constraintsMet = 0;

    int epsilons = 0;
    /**
     * Saves all states that were previously reached with epsilon-transitions.
     * Used for detecting epsilon cycles.
     */
    std::vector<State> epsilonChain;

    [[nodiscard]] std::string getGeneratedString() const {
        std::string result;
        for (const auto& symbol : currentSymbols) {
            result += symbol;
            result += ",";
        }
        return  result.substr(0, result.length() - 1);
    }

    [[nodiscard]] bool isValid() const {
        return currentState != -1;
    }
};

enum class GenerationErrorType {
    NoError,
    ConstraintsNotSatisfiable
};

struct GenerationException : std::exception {
    explicit GenerationException(const GenerationErrorType errorType) : errorType(errorType) {}
    GenerationErrorType errorType;
};

class Generator {
public:
    Generator(const std::map<std::string, GrammarModule>& modules, float maxLength, const EpsilonNFA& nfa, const std::vector<GenerationConstraint>& constraints);

    [[nodiscard]] GenerationResult getGenerationResult() const;
    [[nodiscard]] bool wasGenerationSuccessful() const;
    [[nodiscard]] GenerationErrorType getErrorInfo() const;

private:
    GenerationResult _result;
    GenerationErrorType _errorType;

    float _maxLength;
    const std::map<std::string, GrammarModule>& _modules;
    std::vector<GenerationConstraint> _sortedConstraints;

    [[nodiscard]] GenerationResult generate(const EpsilonNFA& nfa) const;
    void applyTransitionAndAddToQueue(std::queue<GenerationResult>& queue, const GenerationResult& previousResult, const Edge& transition) const;
};
