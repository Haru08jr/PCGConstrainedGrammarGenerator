#include "Generator.hpp"

#include <algorithm>
#include <queue>

#include "utils/UniquePriorityQueue.hpp"

Generator::Generator(const std::map<std::string, GrammarModule>& modules, const float maxLength, const EpsilonNFA& nfa, const std::vector<GenerationConstraint>& constraints)
    : result(-1), errorType(GenerationErrorType::NoError), maxLength(maxLength), modules(modules), sortedConstraints(constraints) {
    // sort the constraints by position along the generation shape
    std::sort(sortedConstraints.begin(), sortedConstraints.end());

    try {
        result = static_cast<GenerationResult>(generate(nfa));
    } catch (const GenerationException& e) {
        errorType = e.errorType;
    }
}

Generator::Generator(const std::map<std::string, GrammarModule>& modules, const float maxLength, const NonEpsilonNFA& nfa, const std::vector<GenerationConstraint>& constraints)
    : result(-1), errorType(GenerationErrorType::NoError), maxLength(maxLength), modules(modules), sortedConstraints(constraints) {
    // sort the constraints by position along the generation shape
    std::sort(sortedConstraints.begin(), sortedConstraints.end());

    try {
        result = generate(nfa);
    } catch (const GenerationException& e) {
        errorType = e.errorType;
    }
}

GenerationResult Generator::getGenerationResult() const {
    return result;
}

bool Generator::wasGenerationSuccessful() const {
    return errorType == GenerationErrorType::NoError;
}

GenerationErrorType Generator::getErrorInfo() const {
    return errorType;
}

GenerationResultWithEpsilons Generator::generate(const EpsilonNFA& nfa) const {
    // queue of intermediate results sorted by least amount of epsilon-transitions & shortest length
    std::priority_queue<GenerationResultWithEpsilons> queue;
    // saves the most optimal result
    GenerationResultWithEpsilons bestResult(-1);

    // Add an empty result at the start state to the queue
    queue.emplace(nfa.getStart());

    while (!queue.empty()) {
        const auto currentResult = queue.top();
        queue.pop();

        if (nfa.getAccept() == currentResult.currentState && currentResult.constraintsMet == sortedConstraints.size()) {
            // if currentResult is longer than the saved best result, save it as the best
            if (bestResult.currentLength < currentResult.currentLength) {
                bestResult = currentResult;
            }
        } else {
            for (const auto& transition: nfa.getAllTransitions(currentResult.currentState)) {
                applyTransitionAndAddToQueue(queue, currentResult, transition);
            }
        }
    }

    // ReSharper disable once CppDFAConstantConditions
    if (!bestResult.isValid())
        throw GenerationException(GenerationErrorType::ConstraintsNotSatisfiable);

    return bestResult;
}

void Generator::applyTransitionAndAddToQueue(std::priority_queue<GenerationResultWithEpsilons>& queue, const GenerationResultWithEpsilons& previousResult, const Edge& transition) const {
    GenerationResultWithEpsilons newResult = previousResult;
    newResult.currentState = transition.getTo();

    if (transition.isEpsilon()) {
        ++newResult.epsilons;
        // if the goal state is saved in the epsilon chain: cycle detected, discard this result
        if (std::ranges::find(newResult.epsilonChain, transition.getTo()) != newResult.epsilonChain.end())
            return;

        newResult.epsilonChain.push_back(newResult.currentState);
    } else {
        newResult.epsilonChain.clear();
        newResult.epsilonChain.push_back(transition.getTo());

        const auto symbol = transition.getLabel();

        // abort in case the NFA contains an unknown symbol
        if (!modules.contains(symbol))
            throw GenerationException(GenerationErrorType::UnknownLiteral);

        bool fulfilledConstraint = false;
        // if there are still open constraints left
        if (newResult.constraintsMet < sortedConstraints.size()) {
            // if placing a symbol that could fulfill the next constraint
            if (sortedConstraints[newResult.constraintsMet].symbol == symbol) {
                // and at the position of the next constraint
                if (newResult.currentLength <= sortedConstraints[newResult.constraintsMet].position &&
                    sortedConstraints[newResult.constraintsMet].position <= newResult.currentLength + modules.at(symbol).size) {
                    // mark this constraint as solved
                    ++newResult.constraintsMet;
                    fulfilledConstraint = true;
                }
            } else if (newResult.currentLength <= sortedConstraints[newResult.constraintsMet].position &&
                       sortedConstraints[newResult.constraintsMet].position < newResult.currentLength + modules.at(symbol).size) {
                // if constraint position is fully covered by an incorrect symbol, this constraint can't be solved anymore -> incorrect result, discard
                return;
            }
        }

        // if placing a symbol that should only be placed with a constraint without fulfilling a constraint, this result is invalid
        if (!fulfilledConstraint && modules.at(symbol).spawnOnlyWithConstraint) {
            return;
        }

        // add the new symbol to the result and increase the result's length
        newResult.currentSymbols.push_back(symbol);
        newResult.currentLength += modules.at(symbol).size;

        // if the result exceeds the maximum length, discard it
        if (newResult.currentLength > maxLength) {
            return;
        }
    }

    queue.push(newResult);
}

GenerationResult Generator::generate(const NonEpsilonNFA& nfa) const {
    // queue of intermediate results sorted by least amount of epsilon-transitions & shortest length
    UniquePriorityQueue<GenerationResult> resultQueue;

    // Fill the queue with all outgoing transitions of the start state
    for (const auto& startState: nfa.getStartStates()) {
        const GenerationResultWithEpsilons start(startState);
        for (const auto& transition: nfa.getAllTransitions(startState)) {
            applyTransitionAndAddToQueue(resultQueue, start, transition);
        }
    }

    // save the most optimal result
    GenerationResult bestResult{-1};

    while (!resultQueue.empty()) {
        const auto currentResult = resultQueue.top();
        resultQueue.pop();

        if (nfa.isAccept(currentResult.currentState) && currentResult.constraintsMet == sortedConstraints.size()) {
            // if currentResult is longer than the saved best result, save it as the best
            if (currentResult.currentLength > bestResult.currentLength) {
                bestResult = currentResult;
            }
        }
        for (const auto& transition: nfa.getAllTransitions(currentResult.currentState)) {
            applyTransitionAndAddToQueue(resultQueue, currentResult, transition);
        }
    }

    return bestResult;
}

void Generator::applyTransitionAndAddToQueue(UniquePriorityQueue<GenerationResult>& queue, const GenerationResult& previousResult, const Edge& transition) const {
    GenerationResult newResult = previousResult;
    newResult.currentState = transition.getTo();

    const auto symbol = transition.getLabel();

    // abort in case the NFA contains an unknown symbol
    if (!modules.contains(symbol))
        throw GenerationException(GenerationErrorType::UnknownLiteral);

    bool fulfilledConstraint = false;
    // if there are still open constraints left
    if (newResult.constraintsMet < sortedConstraints.size()) {
        // if placing a symbol that could fulfill the next constraint
        if (sortedConstraints[newResult.constraintsMet].symbol == symbol) {
            // and at the position of the next constraint
            if (newResult.currentLength <= sortedConstraints[newResult.constraintsMet].position &&
                sortedConstraints[newResult.constraintsMet].position <= newResult.currentLength + modules.at(symbol).size) {
                // mark this constraint as solved
                ++newResult.constraintsMet;
                fulfilledConstraint = true;
            }
        } else if (newResult.currentLength <= sortedConstraints[newResult.constraintsMet].position &&
                   sortedConstraints[newResult.constraintsMet].position < newResult.currentLength + modules.at(symbol).size) {
            // if constraint position is fully covered by an incorrect symbol, this constraint can't be solved anymore -> incorrect result, discard
            return;
        }
    }

    // if placing a symbol that should only be placed with a constraint without fulfilling a constraint, this result is invalid
    if (!fulfilledConstraint && modules.at(symbol).spawnOnlyWithConstraint) {
        return;
    }

    // add the new symbol to the result and increase the result's length
    newResult.currentSymbols.push_back(symbol);
    newResult.currentLength += modules.at(symbol).size;

    // if the result exceeds the maximum length, discard it
    if (newResult.currentLength > maxLength) {
        return;
    }

    queue.push(newResult);
}
