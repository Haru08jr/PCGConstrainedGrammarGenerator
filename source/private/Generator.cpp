#include "Generator.hpp"

#include <algorithm>
#include <queue>

Generator::Generator(const std::map<std::string, GrammarModule>& modules, float maxLength, const NFA& nfa, const std::vector<GenerationConstraint>& constraints)
    : result(-1), errorType(GenerationErrorType::NoError), maxLength(maxLength), modules(modules), nfa(nfa), sortedConstraints(constraints) {
    // sort the constraints by position along the generation shape
    std::sort(sortedConstraints.begin(), sortedConstraints.end());

    try {
        result = generate();
    } catch (const GenerationException& e) {
        errorType = e.errorType;
    }
}

const GenerationResult& Generator::getGenerationResult() const {
    return result;
}

bool Generator::wasGenerationSuccessful() const {
    return errorType == GenerationErrorType::NoError;
}

GenerationErrorType Generator::getErrorInfo() const {
    return errorType;
}

GenerationResult Generator::generate() const {
    // queue of intermediate results sorted by least amount of epsilon-transitions & shortest length
    std::priority_queue<GenerationResult> queue;
    // Fill the queue with all outgoing transitions of the start state
    const GenerationResult start(nfa.getStart());
    for (const auto& transition: nfa.getAllTransitions(nfa.getStart())) {
        applyTransitionAndAddToQueue(queue, start, transition);
    }

    // saves the most optimal result
    GenerationResult bestResult(-1);

    while (!queue.empty()) {
        const auto currentResult = queue.top();
        queue.pop();

        if (nfa.getAccept() == currentResult.currentState && currentResult.constraintsMet == sortedConstraints.size()) {
            // if currentResult is longer than the saved best result, save it as the best
            if (currentResult.currentLength > bestResult.currentLength) {
                bestResult = currentResult;
            }
        } else {
            for (const auto& transition: nfa.getAllTransitions(currentResult.currentState)) {
                applyTransitionAndAddToQueue(queue, currentResult, transition);
            }
        }
    }

    if (!bestResult.isValid())
        throw GenerationException(GenerationErrorType::ConstraintsNotSatisfiable);

    return bestResult;
}

void Generator::applyTransitionAndAddToQueue(std::priority_queue<GenerationResult>& queue, const GenerationResult& previousResult, const Edge& transition) const {
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

        const auto symbol = transition.getLabel();

        // abort in case the NFA contains an unknown symbol
        if (!modules.contains(symbol))
            throw GenerationException(GenerationErrorType::UnknownLiteral);

        // if at the position of the next relevant constraint
        if (newResult.constraintsMet < sortedConstraints.size() &&
            newResult.currentLength <= sortedConstraints[newResult.constraintsMet].position &&
            sortedConstraints[newResult.constraintsMet].position <= newResult.currentLength + modules.at(symbol).size) {
            // if placing the correct character, mark constraint as solved
            if (sortedConstraints[newResult.constraintsMet].symbol == symbol) {
                ++newResult.constraintsMet;
            } else {
                // else this result is invalid, discard it
                return;
            }
        } else if (modules.at(symbol).spawnOnlyWithConstraint) {
            // if the symbol should only be placed with a constraint, discard this result
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
