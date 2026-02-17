#pragma once
#include <map>
#include <set>

#include "Edge.hpp"


class NFA {
public:
    NFA() : start(0), accept(0) {}
    NFA(State start, State accept);
    NFA(const NFA& other);

    void addState(const State& state);

    void addTransition(const Edge& edge);

    void setStart(State state) { start = state; }
    void setAccept(State state) { accept = state; }

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
