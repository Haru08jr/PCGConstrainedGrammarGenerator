#include "Generator.hpp"

#include <algorithm>
#include <queue>

Generator::Generator(const std::map<std::string, GrammarModule>& modules, float maxLength, const NFA& nfa, std::vector<GenerationConstraint> constraints) 
    : result(-1), errorType(GenerationErrorType::NoError)
{
    try {
        result = generate(modules, maxLength, nfa, constraints);
    } catch (const GenerationException& e) {
        errorType = e.errorType;
    }
}

const GenerationResult& Generator::getGenerationResult() const
{
    return result;
}

bool Generator::wasGenerationSuccessful() const
{
    return errorType == GenerationErrorType::NoError;
}

GenerationErrorType Generator::getErrorInfo() const
{
    return errorType;
}

GenerationResult Generator::generate(const std::map<std::string, GrammarModule>& modules, float maxLength, const NFA& nfa, std::vector<GenerationConstraint> constraints) {
    // sort the constraints along the generation shape
    std::sort(constraints.begin(), constraints.end());

    // queue of intermediate results sorted by least amount of epsilon-transitions & shortest length
    std::priority_queue<GenerationResult> queue;
    // Fill the queue with all outgoing transitions of the start state
    const GenerationResult start(nfa.getStart());
    for (const auto& transition: nfa.getAllTransitions(nfa.getStart())) {
        applyTransitionAndAddToQueue(queue, start, transition, modules, constraints);
    }

    // saves the most optimal correct result
    GenerationResult bestResult(-1);

    while (!queue.empty()) {
        const auto currentResult = queue.top();
        queue.pop();

        if (nfa.getAccept() == currentResult.currentState && currentResult.constraintsMet == constraints.size()) {
            // if this result exceeds the maximum length, there will be no better result than the saved one
            if (currentResult.currentLength > maxLength) {
                return bestResult;
            }

            // if currentResult is longer than the saved best result, save it as the best
            if (currentResult.currentLength > bestResult.currentLength) {
                bestResult = currentResult;
            }
        } else {
            for (const auto& transition: nfa.getAllTransitions(currentResult.currentState)) {
                applyTransitionAndAddToQueue(queue, currentResult, transition, modules, constraints);
            }
        }
    }

    if (!bestResult.isValid())
        throw GenerationException(GenerationErrorType::ConstraintsNotSatisfiable);

    return bestResult;
}

void Generator::applyTransitionAndAddToQueue(std::priority_queue<GenerationResult>& queue, const GenerationResult& previousResult, const Edge& transition,
                                             const std::map<std::string, GrammarModule>& modules, std::vector<GenerationConstraint> constraints) {
    GenerationResult newResult = previousResult;
    newResult.currentState = transition.getTo();

    if (transition.isEpsilon()) {
        ++newResult.epsilons;
    } else {
        const auto symbol = transition.getLabel();

        // abort in case the NFA contains an unknown symbol
        if (!modules.contains(symbol))
            throw GenerationException(GenerationErrorType::UnknownLiteral);

        // if at the position of the next relevant constraint
        if (newResult.constraintsMet < constraints.size() &&
            newResult.currentLength <= constraints[newResult.constraintsMet].position &&
            constraints[newResult.constraintsMet].position <= newResult.currentLength + modules.at(symbol).size) {
            // if placing the correct character, mark constraint as solved
            if (constraints[newResult.constraintsMet].symbol == symbol) {
                ++newResult.constraintsMet;
            } else {
                // else this result is invalid, discard it
                return;
            }
        } else if (modules.at(symbol).spawnOnlyWithConstraint) {
            // if the symbol should only be placed with a constraint, discard this result
            return;
        }

        newResult.currentSymbols.push_back(symbol);
        newResult.currentLength += modules.at(symbol).size;
    }

    queue.push(newResult);
}
