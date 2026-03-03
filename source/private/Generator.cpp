#include "../public/Generator.hpp"

#include <algorithm>
#include <queue>

GenerationResult Generator::generate(const std::map<std::string, GrammarModule>& modules, float maxLength, const NFA& nfa, std::vector<GenerationConstraint> constraints) {
    std::vector<GenerationResult> correctResults;
    const float smallestModuleSize = getSmallestModuleSize(modules);

    // sort the constraints along the generation shape
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
                
                // abort in case the NFA contains an unknown symbol
                if (!modules.contains(symbol))
                {
                    return GenerationResult(-1);
                }

                // if at the position of the next relevant constraint
                if (newResult.constraintsMet < constraints.size() &&
                    newResult.currentLength <= constraints[newResult.constraintsMet].position &&
                    constraints[newResult.constraintsMet].position <= newResult.currentLength + modules.at(symbol).size) {

                    // if placing the correct character, mark constraint as solved
                    if (constraints[newResult.constraintsMet].symbol == symbol) {
                        ++newResult.constraintsMet;
                    } else {
                        // else discard
                        continue;
                    }
                }
                else if (modules.at(symbol).spawnOnlyWithConstraint)
                {
                    // if the symbol should only be placed with a constraint, discard this result
                    continue;
                }

                newResult.currentSymbols.push_back(symbol);
                newResult.currentLength += modules.at(symbol).size;
            }

            // discard result if max length is exceeded
            if (newResult.currentLength > maxLength) {
                continue;
            }

            // if at a accepting state & all constraints are met
            if (nfa.getAccept() == newResult.currentState && newResult.constraintsMet == constraints.size()) {
                // if you can't add any more symbols without exceeding the maximum length: found optimal result, return
                if (newResult.currentLength > maxLength - smallestModuleSize) {
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

float Generator::getSmallestModuleSize(const std::map<std::string, GrammarModule>& modules) {
    float smallest = std::numeric_limits<float>::infinity();
    for (const auto& [_, module]: modules) {
        if (smallest > module.size)
            smallest = module.size;
    }
    return smallest;
}
