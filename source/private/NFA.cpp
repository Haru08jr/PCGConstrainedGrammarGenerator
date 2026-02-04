//
// Created by jreut on 28.01.2026.
//

#include "../public/NFA.hpp"

#include <regex>
#include <stack>

NFA::NFA(int numStates) {
    for (int i = 0; i < numStates; i++) {
        states.push_back(i);
    }
    finalState = numStates - 1;
}

NFA::NFA(const std::string& symbol) : NFA(2) {
    addTransition(0, 1, symbol);
}

void NFA::addStates(int numStates) {
    for (int i = 0; i < numStates; i++) {
        states.push_back(i);
    }
}

void NFA::addTransition(int fromState, int toState, const std::string& symbol) {
    transitions.push_back({fromState, toState, symbol});
}

int NFA::getNumStates() const {
    return static_cast<int>(states.size());
}

NFA NFA::concatenate(const NFA& a, const NFA& b) {
    NFA result(a.getNumStates() + b.getNumStates());

    for (const auto& transition: a.transitions) {
        result.addTransition(transition.fromState, transition.toState, transition.symbol);
    }
    result.addTransition(a.finalState, b.states[0], "^"); //TODO what is this symbol?
    for (const auto& transition: b.transitions) {
        result.addTransition(transition.fromState + a.getNumStates(), transition.toState + a.getNumStates(), transition.symbol);
    }

    return result;
}

NFA NFA::kleene(const NFA& original) {
    NFA result(original.getNumStates() + 2);
    result.addTransition(0, 1, "^");

    for (const auto& transition: original.transitions) {
        result.addTransition(transition.fromState + 1, transition.toState + 1, transition.symbol);
    }

    result.addTransition(original.getNumStates(), original.getNumStates() + 1, "^");
    result.addTransition(original.getNumStates(), 1, "^");
    result.addTransition(0, original.getNumStates(), "^");

    return result;
}

/*
NFA NFA::fromRegex(const Regex& regex) {
    std::stack<std::string> operators;
    std::stack<NFA> sections;

    for (const auto& symbol : regex.symbols) {
        if (symbol == "(") {
            operators.push(symbol);
        }else if (symbol == ")") {

        }




        if (symbol == "*") {
            auto toRepeat = sections.top();
            sections.pop();
            sections.push(kleene(toRepeat));
        } else if (symbol == "|" || symbol == "(" || symbol == ".") {
            operators.push(symbol);
        } else if (symbol == ")") {

        }
        // symbol is literal
        {
            sections.emplace(symbol);
        }
    }
    return sections.top();
}
*/