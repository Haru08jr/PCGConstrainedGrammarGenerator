//
// Created by jreut on 10.02.2026.
//

#include "../../public/automaton/NFA.hpp"

NFA::NFA(State start, State accept) : start(start), accept(accept){
    addState(start);
    addState(accept);
}

NFA::NFA(const NFA& other) : start(other.start), accept(other.accept) {
    for (const auto& [state, edges]: other.states) {
        addState(state);
        for (const auto& edge : edges) {
            addTransition(edge);
        }
    }
}

void NFA::addState(const State& name) {
    if (!states.contains(name)) {
        states.insert(std::make_pair(name, std::set<Edge>()));
    }
}

void NFA::addTransition(const Edge& edge) {
    if (states.contains(edge.getFrom()) && states.contains(edge.getTo())) {
        states[edge.getFrom()].insert(edge);
    }
}