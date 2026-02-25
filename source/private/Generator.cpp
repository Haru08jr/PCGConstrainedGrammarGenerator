#include "../public/Generator.hpp"

#include <algorithm>
#include <queue>

GenerationResult Generator::generate(const std::map<std::string, float>& symbolSizes, float maxLength, const NFA& nfa, std::vector<GenerationConstraint> constraints) {
    std::vector<GenerationResult> correctResults;

    std::sort(constraints.begin(), constraints.end());

    std::priority_queue<GenerationResult> queue;
    queue.emplace(nfa.getStart());

    while (!queue.empty()) {
        const auto currentResult = queue.top();
        queue.pop();

        // for each edge going out of the current state
        for (const auto& transition: nfa.getAllTransitions(currentResult.currentState)) {
            GenerationResult newResult = currentResult;
            newResult.currentState = transition.getTo();

            if (transition.isEpsilon())
                ++newResult.epsilons;
            else {
                auto symbol = transition.getLabel();

                // if at the position of the next relevant constraint
                if (newResult.constraintsMet < constraints.size() &&
                    newResult.currentLength <= constraints[newResult.constraintsMet].position /* - constraints[newResult.constraintsMet].halfwith*/ &&
                    constraints[newResult.constraintsMet].position /*+ constraints[newResult.constraintsMet].halfwith*/ <= newResult.currentLength + symbolSizes.at(symbol)) {

                    // if placing the correct character, mark constraint as solved
                    if (constraints[newResult.constraintsMet].symbol == symbol) {
                        ++newResult.constraintsMet;
                    } else {
                        // else discard
                        continue;
                    }
                }

                newResult.currentSymbols.push_back(symbol);
                newResult.currentLength += symbolSizes.at(symbol);
            }

            // discard result if max length is exceeded
            if (newResult.currentLength > maxLength) {
                continue;
            }

            // if at a accepting state & all constraints are met
            if (nfa.getAccept() == newResult.currentState && newResult.constraintsMet == constraints.size()) {
                if (newResult.currentLength == maxLength) {
                    // found optimal result, return
                    return newResult;
                }
                // save result as correct
                correctResults.push_back(newResult);
            }

            // else add back to queue
            queue.emplace(newResult);
        }
    }

    // constraints couldn't be satisfied
    if (correctResults.empty()) {
        return GenerationResult(-1);
    }

    // return the best result found (the longest result with the least amount of epsilons)
    std::sort(correctResults.begin(), correctResults.end(), [](const GenerationResult& a, const GenerationResult& b) {
        if (a.currentLength == b.currentLength) {
            return a.epsilons < b.epsilons;
        }
        return a.currentLength > b.currentLength;
    });
    return correctResults[0];
}
