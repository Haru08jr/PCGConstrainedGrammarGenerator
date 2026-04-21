#include "Generator.hpp"

#include <algorithm>
#include <queue>

#include "utils/UniquePriorityQueue.hpp"

Generator::Generator(const std::map<std::string, GrammarModule>& modules, const float maxLength, const EpsilonNFA& nfa, const std::vector<GenerationConstraint>& constraints)
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

GenerationResult Generator::generate(const EpsilonNFA& nfa) const {
    // queue of intermediate results
    std::queue<GenerationResult> intermediateResults;
    // saves the most optimal result
    GenerationResult bestResult(-1);

    // Add an empty result at the start state to the queue
    intermediateResults.emplace(nfa.getStart());

    while (!intermediateResults.empty()) {
        const auto currentResult = intermediateResults.front();
        intermediateResults.pop();

        if (nfa.getAccept() == currentResult.currentState && currentResult.constraintsMet == sortedConstraints.size()) {
            // if the length of currentResult exactly matches the max length, a solution was found
            if (currentResult.currentLength == maxLength)
                return currentResult;

            // if currentResult is longer than the saved best result, save it as the best
            if (bestResult.currentLength < currentResult.currentLength) {
                bestResult = currentResult;
            }
        } else {
            for (const auto& transition: nfa.getAllTransitions(currentResult.currentState)) {
                applyTransitionAndAddToQueue(intermediateResults, currentResult, transition);
            }
        }
    }

    if (!bestResult.isValid())
        throw GenerationException(GenerationErrorType::ConstraintsNotSatisfiable);

    return bestResult;
}

void Generator::applyTransitionAndAddToQueue(std::queue<GenerationResult>& queue, const GenerationResult& previousResult, const Edge& transition) const {
    GenerationResult newResult = previousResult;
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
