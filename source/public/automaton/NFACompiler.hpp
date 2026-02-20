//
// Created by jreut on 10.02.2026.
//

#pragma once

#include <string>

#include "NFA.hpp"
#include "../regex/RegularExpression.hpp"

struct NFACompilationException : std::exception{
    std::string errorMessage;

    explicit NFACompilationException(std::string errorMessage) : errorMessage(std::move(errorMessage)){}
};

class NFACompiler {
public:
    explicit NFACompiler(const std::shared_ptr<RegularExpression>& regex);
    [[nodiscard]] std::shared_ptr<NFA> getConstructedNFA() const;

private:
    int nextStateLabel = 0;

    const std::shared_ptr<NFA> constructedNFA;
    /** Recursively construct a NFA from a regex. */
    std::unique_ptr<NFA> fromRegex(const std::shared_ptr<RegularExpression>& regex);

    /** Get a new unique state label. */
    State makeNewStateLabel() { return nextStateLabel++; }
    /** Add a copy of all states and transitions in source into dest. */
    static void copyContent(const std::unique_ptr<NFA>& dest, const std::unique_ptr<NFA>& source);
    /** Create a new NFA that is a unique copy of toCopy (= same structure, but no shared states) */
    std::unique_ptr<NFA> makeUniqueCopy(const std::shared_ptr<NFA>& toCopy);

    // NFA construction functions
    /** Create a new NFA with only a start and accept state and no transitions. */
    std::unique_ptr<NFA> makeEmptyNFA();
    /** Create a new NFA with an epsilon edge from start state to accept state. */
    std::unique_ptr<NFA> makeEpsilonNFA();
    /** Create a new NFA with a labled edge from start state to accept state. */
    std::unique_ptr<NFA> makeAtomicNFA(const std::string& label);

    /** Create a new NFA of the structure (start -> (nfa) -> accept). */
    std::unique_ptr<NFA> wrapNFA(const std::unique_ptr<NFA>& toWrap);

    /** Create a new NFA that represents the regex [first,second] */
    static std::unique_ptr<NFA> concatenate(const std::unique_ptr<NFA>& first, const std::unique_ptr<NFA>& second);
    /** Create a new NFA that represents the regex [toRepeat]* */
    std::unique_ptr<NFA> repeat(const std::unique_ptr<NFA>& toRepeat);
    /** Create a new NFA that represents the regex [toRepeat]+ */
    std::unique_ptr<NFA> repeatAtLeastOnce(const std::unique_ptr<NFA>& toRepeat);
    /** Create a new NFA that represents the regex [first|second] */
    std::unique_ptr<NFA> alternative(const std::unique_ptr<NFA>& first, const std::unique_ptr<NFA>& second);
    /** Create a new NFA that represents the regex [optional]? */
    std::unique_ptr<NFA> optional(const std::unique_ptr<NFA>& optional);
    /** Create a new NFA that represents the regex [toRepeat]n */
    std::unique_ptr<NFA> repeatNTimes(std::unique_ptr<NFA>& toRepeat, int n);

};
