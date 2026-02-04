
#pragma once
#include <string>
#include <vector>

#include "RegexPreprocessor.hpp"

struct transition {
    int fromState;
    int toState;
    std::string symbol;
};

class NFA {
public:
    /** Creates a NFA with numStates states and no transitions. The last state will be the final state. */
    explicit NFA(int numStates);

    /** Creates a single-transition NFA. */
    explicit NFA(const std::string& symbol);

    [[nodiscard]] int getNumStates() const;

    static NFA concatenate(const NFA& a, const NFA& b);
    static NFA kleene(const NFA& original);
    //static NFA fromRegex(const Regex& regex);

private:
    std::vector<int> states;
    std::vector<transition> transitions;
    int finalState{};

    void addStates(int numVertices);
    void addTransition(int fromState, int toState, const std::string& symbol);
};
