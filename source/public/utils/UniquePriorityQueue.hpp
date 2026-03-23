#pragma once
#include <queue>
#include <set>

/**
 * A priority queue that does not allow duplicate elements.
 * T needs to implement the < and == operators.
 */
template <typename T>
struct UniquePriorityQueue {
    std::priority_queue<T> priorityQueue;
    std::set<T> set;

    bool push(const T& item) {
        if (contains(item))
            return false;

        set.insert(item);
        priorityQueue.push(item);
        return true;
    }

    bool contains(const T& item) const {
        return set.contains(item);
    }

    T top() const {
        return priorityQueue.top();
    }

    void pop() {
        auto t = top();
        set.erase(t);
        priorityQueue.pop();
    }

    [[nodiscard]] bool empty() const {
        return set.empty();
    }
};
