#ifndef nfa_hpp
#define nfa_hpp
#include <iostream>
#include <unordered_map>
#include <set>
#include <vector>
#include "stack.hpp"
#include "re_tokenizer.hpp"
using namespace std;

typedef int State;

class Edge {
    private:
        State from;
        State to;
    public:
        Edge(State s, State t) : from(s), to(t) { }
        State getFrom() const { return from; }
        State getTo() const { return to; }
        virtual RegExToken getLabel() = 0;
        virtual bool matches(char c) = 0;
        virtual bool positionIs(int index) = 0;
        virtual bool isEpsilon() = 0;
        virtual ~Edge() { }
};

class CharEdge : public Edge {
    private:
        RegExToken label;
        bool checkInRange(char c) {
            char lo, hi;
            bool is_good = false;
            for (int i = 1; i < label.charachters.size()-1; i++) {
                if (i+1 < label.charachters.size() && label.charachters[i] == '-') {
                    lo = label.charachters[i-1];
                    hi = label.charachters[i+1];
                    if (hi < lo) {
                        char tmp = hi;
                        hi = lo;
                        lo = tmp;
                    }
                    if (c >= lo && c <= hi) {
                        is_good = true;
                        break;
                    }
                }
            }
            return is_good;
        }
    public:
        CharEdge(State From, State To, RegExToken c) : Edge(From, To) {
            label = c;
        }
        ~CharEdge() {

        }
        bool isEpsilon() {
            return false;
        }
        bool positionIs(int i) {
            return true;
        }
        bool matches(char c) {
            if (label.symbol == RE_SPECIFIEDSET) {
                for (char m : label.charachters) {
                    if (c == m)
                        return true;
                }
                return false;
            } else if (label.symbol == RE_SPECIFIEDRANGE) {
                return checkInRange(c);
            }
            return label.charachters[0] == c;
        }
        RegExToken getLabel() {
            return label;
        }
};

class EpsilonEdge : public Edge {
    public:
        EpsilonEdge(State From, State To) : Edge(From, To) { }
        ~EpsilonEdge() { }
        bool matches(char c) {
            return true;
        }
        bool positionIs(int i) {
            return true;
        }
        bool isEpsilon() {
            return true;
        }
        RegExToken getLabel() {
            return RegExToken(RE_NONE, "&");
        }
};





bool operator<(const Edge& s, const Edge& t) {
    return s.getFrom() < t.getFrom() && s.getTo() < t.getTo();
}

bool operator==(const Edge& s, const Edge& t) {
    return s.getFrom() == t.getFrom() && s.getTo() == t.getTo();
}

bool operator!=(const Edge& s, const Edge& t) {
    return !(s == t);
}

class NFA {
    private:
        State start;
        State accept;
        unordered_map<State, set<Edge*>> states;
    public:
        NFA() {
            start = 0;
            accept = 0;
        }
        NFA(const NFA& nfa) {
            start = nfa.start;
            accept = nfa.accept;
            for (auto & m : nfa.states) {
                makeState(m.first);
                for (auto & t : m.second) {
                    addTransition(t);
                }
            }
        }
        void makeState(State name) {
            if (states.find(name) == states.end()) {
                states.insert(make_pair(name, set<Edge*>()));
            }
        }
        void setStart(State ss) {
            start = ss;
        }
        void setAccept(State as) {
            accept = as;
        }
        State getStart() {
            return start;
        }
        State getAccept() {
            return accept;
        }
        void addTransition(Edge* t) {
            if (states.at(t->getFrom()).find(t) == states.at(t->getFrom()).end())
                states[t->getFrom()].insert(t);
        }
        int size() {
            return states.size();
        }
        auto getStates() const {
            return states;
        }
        set<Edge*>& getTransitions(State state) {
            return states[state];
        }
        NFA& operator=(const NFA& nfa) {
            if (this != &nfa) {
                start = nfa.start;
                accept = nfa.accept;
                for (auto m : nfa.states) {
                    makeState(m.first);
                    for (auto t : m.second) {
                        addTransition(t);
                    }
                }
            }
            return *this;
        }
};

#endif