
#include "automaton/NFA.hpp"

#include <ranges>

EpsilonNFA::EpsilonNFA(const State start, const State accept) : start(start), accept(accept){
    addState(start);
    addState(accept);
}

EpsilonNFA::EpsilonNFA(const EpsilonNFA& other) : start(other.start), accept(other.accept) {
    for (const auto& state: other.states | std::views::keys) {
        addState(state);
    }
    for (const auto& edges: other.states | std::views::values) {
        for (const auto& edge : edges) {
            addTransition(edge);
        }
    }
}

void EpsilonNFA::addState(const State state) {
    if (!states.contains(state)) {
        states.insert(std::make_pair(state, std::set<Edge>()));
    }
}

void EpsilonNFA::addTransition(const Edge& edge) {
    if (states.contains(edge.getFrom()) && states.contains(edge.getTo())) {
        states[edge.getFrom()].insert(edge);
    }
}