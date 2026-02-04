#ifndef patternmatcher_hpp
#define patternmatcher_hpp
#include <iostream>
#include "stringbuffer.hpp"
#include "NFA/re_compiler.hpp"
#include "NFA/nfa.hpp"
using namespace std;

void printEdge(Edge& t) {
    if (t.isEpsilon()) {
        cout<<'\t'<<t.getFrom()<<" - ["<<t.getLabel().charachters<<"] ->"<<t.getTo()<<endl;
    } else {
        cout<<'\t'<<t.getFrom()<<" - ("<<t.getLabel().charachters<<") ->"<<t.getTo()<<endl;
    }
}

//Algorithm 3.4, Simulating the NFA

class RegExPatternMatcher {
    private:
        NFA nfa;
        // Gathers a list of states reachable from those in 
        // currStates which have transition that consume ch
        set<State> move(set<State> currStates, char ch) {
            set<State> nextStates;
            if (loud) cout<<ch<<": "<<endl;
            for (State s : currStates) {
                for (Edge* t : nfa.getTransitions(s)) {
                    if (t->matches(ch) || t->matches('.')) {
                        if (t->isEpsilon() == false && nextStates.find(t->getTo()) == nextStates.end()) {
                            if (loud) printEdge(*t);
                            nextStates.insert(t->getTo());
                        } 
                    }
                }
            }
            return nextStates;
        }
        //An interesting adaptation of Depth First Search.
        //Gathers a list of states reachable from those in 
        //currStates by using _only_ epsilon transitions.
        set<State> e_closure(set<State> currStates) {
            set<State> nextStates = currStates;
            Stack<State> sf;
            for (State s : currStates)
                sf.push(s);
            while (!sf.empty()) {
                State s = sf.pop();
                for (Edge* t : nfa.getTransitions(s)) {
                    if (t->isEpsilon()) {
                        if (nextStates.find(t->getTo()) == nextStates.end()) {
                            if (loud) printEdge(*t);
                            nextStates.insert(t->getTo());
                            sf.push(t->getTo());
                        }
                    }
                }
            }
            return nextStates;
        }
        int spos;
        string inputText;
        char nextChar() {
            return inputText[spos++];
        }
        char init(const string& text) {
            inputText = text;
            spos = 0;
            return nextChar();
        }
        bool loud;
        const static char eOf = '\0';
    public:
        RegExPatternMatcher(const NFA& fa, bool trace = false) : nfa(fa), loud(trace) {

        }
        RegExPatternMatcher() {
            loud = false;
        }
        void setNFA(NFA& fa) {
            nfa = fa;
        }
        bool match(StringBuffer& sb) {
            set<State> curr = e_closure({nfa.getStart()});
            while (!sb.done()) {
                curr = e_closure(move(curr, sb.get()));
                sb.advance();
            }
            return curr.find(nfa.getAccept()) != curr.end();
        }
        RegExPatternMatcher& printNFA() {
            int t = 0;
            for (auto state : nfa.getStates()) {
                cout<<state.first<<": \n";
                for (auto m : state.second) {
                    cout<<m->getFrom()<<" -> ("<<m->getLabel().charachters<<") -> "<<m->getTo()<<endl;
                }
                t += state.second.size();
            }
            cout<<t<<" transitions."<<endl;
            return *this;
        }
};

struct MatchRE {
    NFACompiler compiler;
    bool operator()(StringBuffer& text, string pattern, bool trace) {
        compiler.setTrace(trace);
        return RegExPatternMatcher(compiler.compile(pattern), trace).printNFA().match(text);
    }
};

#endif