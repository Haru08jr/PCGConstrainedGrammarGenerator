#pragma once
#include <map>
#include <set>

#include "Edge.hpp"

class NFA {
public:
    void addState(State state);
    void addTransition(const Edge& edge);

    [[nodiscard]] int getSize() const { return static_cast<int>(states.size()); }

    [[nodiscard]] const std::map<State, std::set<Edge> >& getAllStates() const { return states; }
    [[nodiscard]] const std::set<Edge>& getAllTransitions(const State state) const { return states.at(state); }

protected:
    std::map<State, std::set<Edge> > states;

    /**
     * Adds the given transition to the NFA.
     * If its start or end state don't exist in the NFA yet, they are added as well.
     */
    void addTransitionWithStates(const Edge& edge);
};

/**
 * Nondeterministic finite automaton that can have epsilon-transitions
 */
class EpsilonNFA : public NFA {
public:
    EpsilonNFA() : _start(0), _accept(0) {
    }

    EpsilonNFA(State start, State accept);

    EpsilonNFA(const EpsilonNFA& other);

    void setStart(const State state) { _start = state; }
    void setAccept(const State state) { _accept = state; }

    [[nodiscard]] State getStart() const { return _start; }
    [[nodiscard]] State getAccept() const { return _accept; }

    /** Finds the epsilon closure (set of states that can be reached using only epsilon transitions) for each state in the NFA. */
    [[nodiscard]] std::map<State, std::set<State> > getEpsilonClosures() const;
    /** Returns only the transitions in the NFA that are labled. */
    [[nodiscard]] std::map<State, std::set<Edge>> getAllLabledTransitions() const;

private:
    State _start;
    State _accept;
};

/**
 * Nondeterministic finite automaton that only has labled transitions
 */
class NonEpsilonNFA : public NFA {
public:
    NonEpsilonNFA() = default;

    explicit NonEpsilonNFA(const EpsilonNFA& epsilonNFA);

    void addStart(State start);
    void addAccept(State accept);

    [[nodiscard]] bool isStart(const State state) const { return _startStates.contains(state); }
    [[nodiscard]] bool isAccept(const State state) const { return _acceptStates.contains(state); }

    [[nodiscard]] const auto& getStartStates() const { return _startStates; }

private:
    std::set<State> _startStates;
    std::set<State> _acceptStates;
};
