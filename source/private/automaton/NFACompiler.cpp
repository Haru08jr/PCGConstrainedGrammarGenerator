//
// Created by jreut on 10.02.2026.
//

#include "../../public/automaton/NFACompiler.hpp"

NFACompiler::NFACompiler(const std::shared_ptr<RegularExpression>& regex){
    try {
        constructedNFA = fromRegex(regex);
    }catch (NFACompilationException& e) {
        errorMessage = e.errorMessage;
    }
}

const NFA& NFACompiler::getConstructedNFA() const {
    return constructedNFA;
}

bool NFACompiler::wasConstructionSuccessful() const {
    return errorMessage.empty();
}

std::string NFACompiler::getErrorMessage() const {
    return errorMessage;
}

NFA NFACompiler::fromRegex(const std::shared_ptr<RegularExpression>& regex) {
    if (!regex || !regex->isValid())
        throw NFACompilationException("Invalid RegularExpression!");

    if (regex->type & Literal) {
        const auto literalRegex = std::static_pointer_cast<LiteralRegex>(regex);
        return makeAtomicNFA(literalRegex->literalString);
    }

    if (regex->type & Unary) {
        const auto unaryRegex = std::static_pointer_cast<UnaryRegex>(regex);
        auto subNFA = fromRegex(unaryRegex->subRegex);

        if (unaryRegex->type == Repeat)
            return repeat(subNFA);

        if (unaryRegex->type == RepeatAtLeastOnce)
            return repeatAtLeastOnce(subNFA);

        if (unaryRegex->type == Option)
            return optional(subNFA);

    }

    if (regex->type & Binary) {
        const auto binaryRegex = std::static_pointer_cast<BinaryRegex>(regex);
        auto firstSubNFA = fromRegex(binaryRegex->firstSubRegex);
        auto secondSubNFA = fromRegex(binaryRegex->secondSubRegex);

        if (binaryRegex->type == Concatenation)
            return concatenate(firstSubNFA, secondSubNFA);

        if (binaryRegex->type == Alternative)
            return alternative(firstSubNFA, secondSubNFA);

    }

    if (regex->type & RepeatNTimes) {
        const auto paramRegex = std::static_pointer_cast<UnaryRegexWithParam>(regex);
        auto subNFA = fromRegex(paramRegex->subRegex);

        return repeatNTimes(subNFA, paramRegex->parameter);
    }
    
    throw NFACompilationException("NFA could not be constructed!");
}

void NFACompiler::copyContent(NFA& dest, const NFA& source) {
    for (const auto& [state, transitions] : source.getAllStates()) {
        dest.addState(state);
    }
    for (const auto& [state, transitions] : source.getAllStates()) {
        for (const auto& transition : transitions) {
            dest.addTransition(transition);
        }
    }
}

NFA NFACompiler::makeUniqueCopy(const NFA& toCopy) {
    auto nfa = makeEmptyNFA();

    // Matches state in toCopy to newly created state
    std::map<State, State> stateLookup;

    for (const auto& [state, transitions] : toCopy.getAllStates()) {
        State newState = makeNewStateLabel();
        stateLookup.emplace(state, newState);
        nfa.addState(newState);
    }
    for (const auto& [state, transitions] : toCopy.getAllStates()) {
        for (const auto& transition : transitions) {
            if (transition.isEpsilon())
                nfa.addTransition({stateLookup[transition.getFrom()], stateLookup[transition.getTo()]});
            else
                nfa.addTransition({stateLookup[transition.getFrom()], stateLookup[transition.getTo()], transition.getLabel()});
        }
    }
    nfa.setStart(stateLookup[toCopy.getStart()]);
    nfa.setAccept(stateLookup[toCopy.getAccept()]);

    return nfa;
}

NFA NFACompiler::makeEmptyNFA() {
    return {makeNewStateLabel(), makeNewStateLabel()};
}

NFA NFACompiler::makeEpsilonNFA() {
    auto nfa = makeEmptyNFA();
    nfa.addTransition({nfa.getStart(), nfa.getAccept()});
    return nfa;
}

NFA NFACompiler::makeAtomicNFA(const std::string& label) {
    auto nfa = makeEmptyNFA();
    nfa.addTransition({nfa.getStart(), nfa.getAccept(), label});
    return nfa;
}

NFA NFACompiler::wrapNFA(const NFA& toWrap) {
    // construct (start -> (nfa) -> accept)
    auto nfa = makeEmptyNFA();
    copyContent(nfa, toWrap);
    nfa.addTransition({nfa.getStart(), toWrap.getStart()});
    nfa.addTransition({toWrap.getAccept(), nfa.getAccept()});

    return nfa;
}

NFA NFACompiler::concatenate(const NFA& first, const NFA& second) {
    auto nfa{first};
    copyContent(nfa, second);
    nfa.setStart(first.getStart());
    nfa.setAccept(second.getAccept());
    nfa.addTransition({first.getAccept(), second.getStart()});

    return nfa;
}

NFA NFACompiler::repeat(const NFA& toRepeat) {
    // construct NFA that repeats toRepeat at least once
    auto nfa = repeatAtLeastOnce(toRepeat);

    // add a direct transition from start to accept to ignore the repetition
    nfa.addTransition({nfa.getStart(), nfa.getAccept()});
    return nfa;
}

NFA NFACompiler::repeatAtLeastOnce(const NFA& toRepeat) {
    // construct (start -> (toRepeat) -> accept)
    auto nfa = wrapNFA(toRepeat);

    // loop back from toRepeat.accept to start for repetition
    nfa.addTransition({toRepeat.getAccept(), nfa.getStart()});

    return nfa;
}

NFA NFACompiler::alternative(const NFA& first, const NFA& second) {
    // construct (start  (first) (second) accept)
    auto nfa = makeEmptyNFA();
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
    auto nfa = wrapNFA(optional);

    // add a direct transition from start to accept for ignoring
    nfa.addTransition({nfa.getStart(), nfa.getAccept()});

    return nfa;
}

NFA NFACompiler::repeatNTimes(const NFA& toRepeat, int n) {
    
   auto previous = makeUniqueCopy(toRepeat);

    for (int i = 1; i < n; i++) {
        auto temp = makeUniqueCopy(toRepeat);
        previous = concatenate(previous, temp);
    }
    auto nfa = wrapNFA(previous);

    return nfa;
}
