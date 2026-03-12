
#include "automaton/NFACompiler.hpp"

#include <ranges>

NFACompiler::NFACompiler(const std::shared_ptr<RegularExpression>& regex) : constructionError(NFAErrorType::NoError){
    try {
        constructedNFA = fromRegex(regex);
    }catch (NFACompilationException& e) {
        constructionError = e.errorType;
    }
}

const EpsilonNFA& NFACompiler::getConstructedNFA() const {
    return constructedNFA;
}

bool NFACompiler::wasConstructionSuccessful() const {
    return constructionError == NFAErrorType::NoError;
}

NFAErrorType NFACompiler::getErrorInfo() const {
    return constructionError;
}

EpsilonNFA NFACompiler::fromRegex(const std::shared_ptr<RegularExpression>& regex) {
    if (!regex)
        throw NFACompilationException(NFAErrorType::EmptyRegex);

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
    
    throw NFACompilationException(NFAErrorType::InvalidRegex);
}

void NFACompiler::copyContent(EpsilonNFA& dest, const EpsilonNFA& source) {
    for (const auto& state: source.getAllStates() | std::views::keys) {
        dest.addState(state);
    }
    for (const auto& transitions: source.getAllStates() | std::views::values) {
        for (const auto& transition : transitions) {
            dest.addTransition(transition);
        }
    }
}

EpsilonNFA NFACompiler::makeUniqueCopy(const EpsilonNFA& toCopy) {
    auto nfa = makeEmptyNFA();

    // Matches state in toCopy to newly created state
    std::map<State, State> stateLookup;

    for (const auto& state: toCopy.getAllStates() | std::views::keys) {
        State newState = makeNewStateLabel();
        stateLookup.emplace(state, newState);
        nfa.addState(newState);
    }
    for (const auto& transitions: toCopy.getAllStates() | std::views::values) {
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

EpsilonNFA NFACompiler::makeEmptyNFA() {
    return {makeNewStateLabel(), makeNewStateLabel()};
}

EpsilonNFA NFACompiler::makeEpsilonNFA() {
    auto nfa = makeEmptyNFA();
    nfa.addTransition({nfa.getStart(), nfa.getAccept()});
    return nfa;
}

EpsilonNFA NFACompiler::makeAtomicNFA(const std::string& label) {
    auto nfa = makeEmptyNFA();
    nfa.addTransition({nfa.getStart(), nfa.getAccept(), label});
    return nfa;
}

EpsilonNFA NFACompiler::wrapNFA(const EpsilonNFA& toWrap) {
    // construct (start -> (nfa) -> accept)
    auto nfa = makeEmptyNFA();
    copyContent(nfa, toWrap);
    nfa.addTransition({nfa.getStart(), toWrap.getStart()});
    nfa.addTransition({toWrap.getAccept(), nfa.getAccept()});

    return nfa;
}

EpsilonNFA NFACompiler::concatenate(const EpsilonNFA& first, const EpsilonNFA& second) {
    auto nfa{first};
    copyContent(nfa, second);
    nfa.setStart(first.getStart());
    nfa.setAccept(second.getAccept());
    nfa.addTransition({first.getAccept(), second.getStart()});

    return nfa;
}

EpsilonNFA NFACompiler::repeat(const EpsilonNFA& toRepeat) {
    // construct NFA that repeats toRepeat at least once
    auto nfa = repeatAtLeastOnce(toRepeat);

    // add a direct transition from start to accept to ignore the repetition
    nfa.addTransition({nfa.getStart(), nfa.getAccept()});
    return nfa;
}

EpsilonNFA NFACompiler::repeatAtLeastOnce(const EpsilonNFA& toRepeat) {
    // construct (start -> (toRepeat) -> accept)
    auto nfa = wrapNFA(toRepeat);

    // loop back from toRepeat.accept to start for repetition
    nfa.addTransition({toRepeat.getAccept(), nfa.getStart()});

    return nfa;
}

EpsilonNFA NFACompiler::alternative(const EpsilonNFA& first, const EpsilonNFA& second) {
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

EpsilonNFA NFACompiler::optional(const EpsilonNFA& optional) {
    // construct (start -> (optional) -> accept)
    auto nfa = wrapNFA(optional);

    // add a direct transition from start to accept for ignoring
    nfa.addTransition({nfa.getStart(), nfa.getAccept()});

    return nfa;
}

EpsilonNFA NFACompiler::repeatNTimes(const EpsilonNFA& toRepeat, int n) {
    
   auto previous = makeUniqueCopy(toRepeat);

    for (int i = 1; i < n; i++) {
        auto temp = makeUniqueCopy(toRepeat);
        previous = concatenate(previous, temp);
    }
    auto nfa = wrapNFA(previous);

    return nfa;
}
