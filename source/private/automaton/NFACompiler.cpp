//
// Created by jreut on 10.02.2026.
//

#include "../../public/automaton/NFACompiler.hpp"

void NFACompiler::fromRegex(const RegularExpression& regex, NFA& outNFA) {
    if (!regex.isValid())
        return;

    if (regex.type & Literal) {
        auto& literalRegex = static_cast<const LiteralRegex&>(regex);
        outNFA = makeAtomicNFA(literalRegex.literalString);
        return;
    }

    if (regex.type & Unary) {
        auto& unaryRegex = static_cast<const UnaryRegex&>(regex);
        NFA subNFA;
        fromRegex(*unaryRegex.subRegex, subNFA);

        if (unaryRegex.type == Repeat) {
            outNFA = repeat(subNFA);
        }else if (unaryRegex.type == RepeatAtLeastOnce) {
            outNFA = repeatAtLeastOnce(subNFA);
        }else if (unaryRegex.type == Option) {
            outNFA = optional(subNFA);
        }

        return;
    }

    if (regex.type & Binary) {
        auto& binaryRegex = static_cast<const BinaryRegex&>(regex);
        NFA firstSubNFA;
        fromRegex(*binaryRegex.firstSubRegex, firstSubNFA);
        NFA secondSubNFA;
        fromRegex(*binaryRegex.firstSubRegex, secondSubNFA);

        if (binaryRegex.type == Concatenation) {
            outNFA = concatenate(firstSubNFA, secondSubNFA);
        }else if (binaryRegex.type == Alternative) {
            outNFA = alternative(firstSubNFA, secondSubNFA);
        }

        return;
    }

    if (regex.type & RepeatNTimes) {
        auto& paramRegex = static_cast<const UnaryRegexWithParam&>(regex);
        NFA subNFA;
        fromRegex(*paramRegex.subRegex, subNFA);

        outNFA = repeatNTimes(subNFA, paramRegex.parameter);

        return;
    }
}

void NFACompiler::copyContent(NFA& dest, const NFA& source) {
    for (const auto& [state, transitions] : source.getAllStates()) {
        dest.addState(state);
        for (const auto& transition : transitions) {
            dest.addTransition(transition);
        }
    }
}

NFA NFACompiler::makeEmptyNFA() {
    return {makeNewStateLabel(), makeNewStateLabel()};
}

NFA NFACompiler::makeEpsilonNFA() {
    NFA nfa = makeEmptyNFA();
    nfa.addTransition({nfa.getStart(), nfa.getAccept()});
    return nfa;
}

NFA NFACompiler::makeAtomicNFA(const std::string& label) {
    NFA nfa = makeEmptyNFA();
    nfa.addTransition({nfa.getStart(), nfa.getAccept(), label});
    return nfa;
}

NFA NFACompiler::wrapNFA(const NFA& toWrap) {
    // construct (start -> (nfa) -> accept)
    NFA nfa = makeEmptyNFA();
    copyContent(nfa, toWrap);
    nfa.addTransition({nfa.getStart(), toWrap.getStart()});
    nfa.addTransition({toWrap.getAccept(), nfa.getAccept()});

    return nfa;
}

NFA NFACompiler::concatenate(const NFA& first, const NFA& second) {
    // construct ((first) -> (second))
    NFA nfa{first};
    copyContent(nfa, second);
    nfa.setStart(first.getStart());
    nfa.setAccept(second.getAccept());
    nfa.addTransition({first.getAccept(), second.getStart()});

    return nfa;
}

NFA NFACompiler::repeat(const NFA& toRepeat) {
    // construct NFA that repeats toRepeat at least once
    NFA nfa = repeatAtLeastOnce(toRepeat);

    // add a direct transition from start to accept to ignore the repetition
    nfa.addTransition({nfa.getStart(), nfa.getAccept()});
    return nfa;
}

NFA NFACompiler::repeatAtLeastOnce(const NFA& toRepeat) {
    // construct (start -> (toRepeat) -> accept)
    NFA nfa = wrapNFA(toRepeat);

    // loop back from toRepeat.accept to start for repetition
    nfa.addTransition({toRepeat.getAccept(), nfa.getStart()});

    return nfa;
}

NFA NFACompiler::alternative(const NFA& first, const NFA& second) {
    // construct (start  (first) (second) accept)
    NFA nfa = makeEmptyNFA();
    copyContent(nfa, first);
    copyContent(nfa, second);

    // connect (start -> (first.start ... first.accept) -> accept)
    nfa.addTransition({nfa.getStart(), first.getStart()});
    nfa.addTransition({first.getAccept(), nfa.getAccept()});

    // connect (start -> (second.start ... second.accept) -> accept)
    nfa.addTransition({nfa.getStart(), second.getStart()});
    nfa.addTransition({second.getAccept(), nfa.getAccept()});

    return nfa;
}

NFA NFACompiler::optional(const NFA& optional) {
    // construct (start -> (optional) -> accept)
    NFA nfa = wrapNFA(optional);

    // add a direct transition from start to accept for ignoring
    nfa.addTransition({nfa.getStart(), nfa.getAccept()});

    return nfa;
}

NFA NFACompiler::repeatNTimes(const NFA& toRepeat, int n) {
    NFA nfa = makeEmptyNFA();

    State previous = nfa.getStart();
    for (int i = 0; i < n; i++) {
        // previous -> temp.start -> (toRepeat) -> temp.accept
        NFA temp = wrapNFA(toRepeat);
        copyContent(nfa, temp);
        nfa.addTransition({previous, temp.getStart()});

        previous = temp.getAccept();
    }
    // connect final previous to accept state
    nfa.addTransition({previous, nfa.getAccept()});

    return nfa;
}
