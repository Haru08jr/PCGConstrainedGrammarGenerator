#pragma once

#include <string>

#include "NFA.hpp"
#include "regex/RegularExpression.hpp"

enum class NFAErrorType {
    NoError,
    EmptyRegex,
    InvalidRegex
};

struct NFACompilationException : std::exception{
    NFAErrorType errorType;
    explicit NFACompilationException(const NFAErrorType errorType) : errorType(errorType){}
};

/**
 * Used to construct a NFA from a RegularExpression tree.
 * NFACompilers are NFA-specific, which means that for each new NFA a new compiler has to be created.
 */
class NFACompiler {
public:
    explicit NFACompiler(const std::shared_ptr<RegularExpression>& regex);

    [[nodiscard]] const EpsilonNFA& getConstructedNFA() const;
    [[nodiscard]] bool wasConstructionSuccessful() const;
    [[nodiscard]] NFAErrorType getErrorInfo() const;

private:
    int nextStateLabel = 0;

    EpsilonNFA constructedNFA;
    NFAErrorType constructionError;

    /** Recursively construct a NFA from a regex. */
    EpsilonNFA fromRegex(const std::shared_ptr<RegularExpression>& regex);

    /** Get a new unique state label. */
    State makeNewStateLabel() { return nextStateLabel++; }
    /** Add a copy of all states and transitions in source into dest. */
    static void copyContent(EpsilonNFA& dest, const EpsilonNFA& source);
    /** Create a new NFA that is a unique copy of toCopy (= same structure, but no shared states) */
    EpsilonNFA makeUniqueCopy(const EpsilonNFA& toCopy);

    // NFA construction functions
    /** Create a new NFA with only a start and accept state and no transitions. */
    EpsilonNFA makeEmptyNFA();
    /** Create a new NFA with an epsilon edge from start state to accept state. */
    EpsilonNFA makeEpsilonNFA();
    /** Create a new NFA with a labled edge from start state to accept state. */
    EpsilonNFA makeAtomicNFA(const std::string& label);

    /** Create a new NFA of the structure (start -> (nfa) -> accept). */
    EpsilonNFA wrapNFA(const EpsilonNFA& toWrap);

    /** Create a new NFA that represents the regex [first,second] */
    static EpsilonNFA concatenate(const EpsilonNFA& first, const EpsilonNFA& second);
    /** Create a new NFA that represents the regex [toRepeat]* */
    EpsilonNFA repeat(const EpsilonNFA& toRepeat);
    /** Create a new NFA that represents the regex [toRepeat]+ */
    EpsilonNFA repeatAtLeastOnce(const EpsilonNFA& toRepeat);
    /** Create a new NFA that represents the regex [first|second] */
    EpsilonNFA alternative(const EpsilonNFA& first, const EpsilonNFA& second);
    /** Create a new NFA that represents the regex [optional]? */
    EpsilonNFA optional(const EpsilonNFA& optional);
    /** Create a new NFA that represents the regex [toRepeat]n */
    EpsilonNFA repeatNTimes(const EpsilonNFA& toRepeat, int n);

};
