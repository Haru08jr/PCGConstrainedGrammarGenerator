#pragma once
#include <utility>
#include <string>

typedef int State;

class Edge {
public:
    Edge(State s, State t) : from(s), to(t), isEpsilonEdge(true) {
    }

    Edge(State s, State t, std::string label) : from(s), to(t), isEpsilonEdge(false), label(std::move(label)) {}

    Edge(const Edge& other) : from(other.from), to(other.to), isEpsilonEdge(other.isEpsilonEdge), label(other.getLabel()){}

    virtual ~Edge() = default;

    [[nodiscard]] State getFrom() const { return from; }
    [[nodiscard]] State getTo() const { return to; }

    [[nodiscard]] bool isEpsilon() const { return isEpsilonEdge; }
    [[nodiscard]] std::string getLabel() const { return label; }

    bool operator<(const Edge& other) const {
        return from < other.from
            || from == other.from && to < other.to
            || from == other.from && to == other.to && getLabel() < other.getLabel();
    }

    bool operator==(const Edge& other) const {
        return from == other.from && to == other.to && isEpsilonEdge == other.isEpsilonEdge && label == other.label;
    }

private:
    State from;
    State to;
    bool isEpsilonEdge;
    std::string label;
};