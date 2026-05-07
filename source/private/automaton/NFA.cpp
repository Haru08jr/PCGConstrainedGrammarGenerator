
#include "automaton/NFA.hpp"

#include <ranges>

void NFA::addState(const State state) {
    if (!states.contains(state)) {
        states.insert(std::make_pair(state, std::set<Edge>()));
    }
}

void NFA::addTransition(const Edge& edge) {
    if (states.contains(edge.getFrom()) && states.contains(edge.getTo())) {
        states[edge.getFrom()].insert(edge);
    }
}

void NFA::addTransitionWithStates(const Edge& edge) {
    addState(edge.getFrom());
    addState(edge.getTo());
    addTransition(edge);
}

EpsilonNFA::EpsilonNFA(const State start, const State accept) : _start(start), _accept(accept){
    addState(start);
    addState(accept);
}

EpsilonNFA::EpsilonNFA(const EpsilonNFA& other) : _start(other._start), _accept(other._accept) {
    for (const auto& state: other.states | std::views::keys) {
        addState(state);
    }
    for (const auto& edges: other.states | std::views::values) {
        for (const auto& edge : edges) {
            addTransition(edge);
        }
    }
}

std::map<State, std::set<State>> EpsilonNFA::getEpsilonClosures() const {
    std::map<State, std::set<State>> closures;

    // for each state in the NFA, find the states that can be reached with one epsilon-transition
    for (const auto& [state, transitions]: states) {
        auto& closure = closures[state];

        for (const auto& transition: transitions) {
            if (transition.isEpsilon())
                closure.insert(transition.getTo());
        }
    }

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& [state, reachableStates]: closures) {
            // From each reachable state, get the reachable states of this state and add them to the closure.
            for (auto reachableState : reachableStates) {
                for (auto secondDegreeReachable : closures[reachableState]) {
                    const auto& [_, insertionSuccessful] = closures[state].insert(secondDegreeReachable);
                    if (insertionSuccessful)
                        changed = true;
                }
            }
        }
        // Repeat this until there is no new state that can be reached.
    }
    return closures;
}

std::map<State, std::set<Edge>> EpsilonNFA::getAllLabledTransitions() const {
    std::map<State, std::set<Edge>> labledTransitions;

    for (const auto& [state, edges]: getAllStates()) {
        auto& transitions = labledTransitions[state];

        for (const auto& edge : edges) {
            if (!edge.isEpsilon())
                transitions.insert(edge);
        }
    }
    return labledTransitions;
}


NonEpsilonNFA::NonEpsilonNFA(const EpsilonNFA& epsilonNFA) {
    const auto labledTransitions = epsilonNFA.getAllLabledTransitions();
    const auto epsilonClosures = epsilonNFA.getEpsilonClosures();

    for (const auto& [state, closure]: epsilonClosures) {
        // add all labled transitions to the NFA
        for (const auto& transition: labledTransitions.at(state)) {
            addTransitionWithStates(transition);
        }

        for (const auto stateInClosure: closure) {
            // for each state in the epsilon closure, if it has an outgoing labled transition, the original state gets this transition.
            for (const auto& transition : labledTransitions.at(stateInClosure))
                addTransitionWithStates({state, transition.getTo(), transition.getLabel()});
            // if the state is an accept state in the epsilon-NFA, the original state will also be an accept state.
            if (stateInClosure == epsilonNFA.getAccept())
                addAccept(state);
        }
    }

    addAccept(epsilonNFA.getAccept());
    addStart(epsilonNFA.getStart());

    // all states in the epsilon closure of the epsilon-NFAs start state become start states as well.
    for (const auto state : epsilonClosures.at(epsilonNFA.getStart())) {
        addStart(state);
    }
}

void NonEpsilonNFA::addStart(const State start) {
    if (states.contains(start))
        _startStates.insert(start);
}

void NonEpsilonNFA::addAccept(const State accept) {
    if (states.contains(accept))
        _acceptStates.insert(accept);
}
