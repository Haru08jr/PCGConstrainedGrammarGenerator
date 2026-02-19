//
// Created by jreut on 10.02.2026.
//

#include "../../public/automaton/NFACompiler.hpp"

NFACompiler::NFACompiler(const std::shared_ptr<RegularExpression>& regex) :constructedNFA(fromRegex(regex)){
}

std::shared_ptr<NFA> NFACompiler::getConstructedNFA() const {
    return constructedNFA;
}

std::unique_ptr<NFA> NFACompiler::fromRegex(const std::shared_ptr<RegularExpression>& regex) {
    if (!regex->isValid())
        return nullptr;

    if (regex->type & Literal) {
        const auto literalRegex = std::static_pointer_cast<LiteralRegex>(regex);
        return makeAtomicNFA(literalRegex->literalString);
    }

    if (regex->type & Unary) {
        const auto unaryRegex = std::static_pointer_cast<UnaryRegex>(regex);
        auto subNFA = fromRegex(unaryRegex->subRegex);

        if (unaryRegex->type == Repeat)
            return repeat(std::move(subNFA));

        if (unaryRegex->type == RepeatAtLeastOnce)
            return repeatAtLeastOnce(std::move(subNFA));

        if (unaryRegex->type == Option)
            return optional(std::move(subNFA));

    }

    if (regex->type & Binary) {
        const auto binaryRegex = std::static_pointer_cast<BinaryRegex>(regex);
        auto firstSubNFA = fromRegex(binaryRegex->firstSubRegex);
        auto secondSubNFA = fromRegex(binaryRegex->firstSubRegex);

        if (binaryRegex->type == Concatenation)
            return concatenate(std::move(firstSubNFA), std::move(secondSubNFA));

        if (binaryRegex->type == Alternative)
            return alternative(std::move(firstSubNFA), std::move(secondSubNFA));

    }

    if (regex->type & RepeatNTimes) {
        const auto paramRegex = std::static_pointer_cast<UnaryRegexWithParam>(regex);
        auto subNFA = fromRegex(paramRegex->subRegex);

        return repeatNTimes(std::move(subNFA), paramRegex->parameter);
    }

    return nullptr;
}

void NFACompiler::copyContent(const std::unique_ptr<NFA>& dest, const std::shared_ptr<NFA>& source) {
    for (const auto& [state, transitions] : source->getAllStates()) {
        dest->addState(state);
        for (const auto& transition : transitions) {
            dest->addTransition(transition);
        }
    }
}

std::unique_ptr<NFA> NFACompiler::makeEmptyNFA() {
    return std::make_unique<NFA>(makeNewStateLabel(), makeNewStateLabel());
}

std::unique_ptr<NFA> NFACompiler::makeEpsilonNFA() {
    auto nfa = makeEmptyNFA();
    nfa->addTransition({nfa->getStart(), nfa->getAccept()});
    return nfa;
}

std::unique_ptr<NFA> NFACompiler::makeAtomicNFA(const std::string& label) {
    auto nfa = makeEmptyNFA();
    nfa->addTransition({nfa->getStart(), nfa->getAccept(), label});
    return nfa;
}

std::unique_ptr<NFA> NFACompiler::wrapNFA(const std::shared_ptr<NFA>& toWrap) {
    // construct (start -> (nfa) -> accept)
    auto nfa = makeEmptyNFA();
    copyContent(nfa, toWrap);
    nfa->addTransition({nfa->getStart(), toWrap->getStart()});
    nfa->addTransition({toWrap->getAccept(), nfa->getAccept()});

    return nfa;
}

std::unique_ptr<NFA> NFACompiler::concatenate(const std::shared_ptr<NFA>& first, const std::shared_ptr<NFA>& second) {
    auto nfa = std::make_unique<NFA>(*first);
    copyContent(nfa, second);
    nfa->setStart(first->getStart());
    nfa->setAccept(second->getAccept());
    nfa->addTransition({first->getAccept(), second->getStart()});

    return nfa;
}

std::unique_ptr<NFA> NFACompiler::repeat(const std::shared_ptr<NFA>& toRepeat) {
    // construct NFA that repeats toRepeat at least once
    auto nfa = repeatAtLeastOnce(toRepeat);

    // add a direct transition from start to accept to ignore the repetition
    nfa->addTransition({nfa->getStart(), nfa->getAccept()});
    return nfa;
}

std::unique_ptr<NFA> NFACompiler::repeatAtLeastOnce(const std::shared_ptr<NFA>& toRepeat) {
    // construct (start -> (toRepeat) -> accept)
    auto nfa = wrapNFA(toRepeat);

    // loop back from toRepeat.accept to start for repetition
    nfa->addTransition({toRepeat->getAccept(), nfa->getStart()});

    return nfa;
}

std::unique_ptr<NFA> NFACompiler::alternative(const std::shared_ptr<NFA>& first, const std::shared_ptr<NFA>& second) {
    // construct (start  (first) (second) accept)
    auto nfa = makeEmptyNFA();
    copyContent(nfa, first);
    copyContent(nfa, second);

    // connect (start -> (first.start ... first.accept) -> accept)
    nfa->addTransition({nfa->getStart(), first->getStart()});
    nfa->addTransition({first->getAccept(), nfa->getAccept()});

    // connect (start -> (second.start ... second.accept) -> accept)
    nfa->addTransition({nfa->getStart(), second->getStart()});
    nfa->addTransition({second->getAccept(), nfa->getAccept()});

    return nfa;
}

std::unique_ptr<NFA> NFACompiler::optional(const std::shared_ptr<NFA>& optional) {
    // construct (start -> (optional) -> accept)
    auto nfa = wrapNFA(optional);

    // add a direct transition from start to accept for ignoring
    nfa->addTransition({nfa->getStart(), nfa->getAccept()});

    return nfa;
}

std::unique_ptr<NFA>  NFACompiler::repeatNTimes(const std::shared_ptr<NFA>& toRepeat, int n) {
    auto nfa = makeEmptyNFA();

    State previous = nfa->getStart();
    for (int i = 0; i < n; i++) {
        // previous -> temp.start -> (toRepeat) -> temp.accept
        auto temp = wrapNFA(toRepeat);
        copyContent(nfa, std::move(temp));
        nfa->addTransition({previous, temp->getStart()});

        previous = temp->getAccept();
    }
    // connect final previous to accept state
    nfa->addTransition({previous, nfa->getAccept()});

    return nfa;
}
