#pragma once
#include <map>
#include <set>

#include "Edge.hpp"

class EpsilonNFA {
public:
    EpsilonNFA() : start(0), accept(0) {}
    EpsilonNFA(State start, State accept);
    EpsilonNFA(const EpsilonNFA& other);

    void addState(State state);

    void addTransition(const Edge& edge);

    void setStart(const State state) { start = state; }
    void setAccept(const State state) { accept = state; }

    [[nodiscard]] State getStart() const { return start; }
    [[nodiscard]] State getAccept() const { return accept; }

    [[nodiscard]] int getSize() const { return static_cast<int>(states.size()); }

    [[nodiscard]] const std::map<State, std::set<Edge>>& getAllStates() const { return states; }
    [[nodiscard]] const std::set<Edge>& getAllTransitions(State state) const { return states.at(state); }

private:
    State start;
    State accept;
    std::map<State, std::set<Edge>> states;
};
