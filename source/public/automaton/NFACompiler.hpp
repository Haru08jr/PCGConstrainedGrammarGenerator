//
// Created by jreut on 10.02.2026.
//

#pragma once

#include <string>

#include "NFA.hpp"
#include "../regex/RegularExpression.hpp"

class NFACompiler {
public:
    void fromRegex(const std::shared_ptr<RegularExpression>& regex, NFA& outNFA);
    //NFA fromRegex(const RegularExpression& regex);

private:
    int nextStateLabel = 0;

    /** Get a new unique state label. */
    State makeNewStateLabel() { return nextStateLabel++; }
    /** Add a copy of all states and transitions in source into dest. */
    static void copyContent(NFA& dest, const NFA& source);

    // NFA construction functions
    /** Create a new NFA with only a start and accept state and no transitions. */
    NFA makeEmptyNFA();
    /** Create a new NFA with an epsilon edge from start state to accept state. */
    NFA makeEpsilonNFA();
    /** Create a new NFA with a labled edge from start state to accept state. */
    NFA makeAtomicNFA(const std::string& label);

    /** Create a new NFA of the structure (start -> (nfa) -> accept). */
    NFA wrapNFA(const NFA& toWrap);

    /** Create a new NFA that represents the regex [first,second] */
    static NFA concatenate(const NFA& first, const NFA& second);
    /** Create a new NFA that represents the regex [toRepeat]* */
    NFA repeat(const NFA& toRepeat);
    /** Create a new NFA that represents the regex [toRepeat]+ */
    NFA repeatAtLeastOnce(const NFA& toRepeat);
    /** Create a new NFA that represents the regex [first|second] */
    NFA alternative(const NFA& first, const NFA& second);
    /** Create a new NFA that represents the regex [optional]? */
    NFA optional(const NFA& optional);
    /** Create a new NFA that represents the regex [toRepeat]n */
    NFA repeatNTimes(const NFA& toRepeat, int n);

};
