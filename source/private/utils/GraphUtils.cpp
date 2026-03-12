
#include <ranges>
#include <utils/GraphUtils.hpp>
#include <automaton/NFA.hpp>

#include "graaflib/io/dot.h"

NFAGraph::NFAGraph(const EpsilonNFA& nfa) {
    std::map<State, graaf::vertex_id_t> stateMapping;
    for (const auto& state: nfa.getAllStates() | std::views::keys) {
        GraphVertex v(state, nfa.getStart() == state, nfa.getAccept() == state);
        stateMapping.emplace(state, graph.add_vertex(v));
    }
    for (const auto& edges: nfa.getAllStates() | std::views::values) {
        for (const auto& edge : edges) {
            graph.add_edge(stateMapping[edge.getFrom()], stateMapping[edge.getTo()], edge);
        }
    }
}

void NFAGraph::printGraph(const std::string& filename) const {
    graaf::io::to_dot(graph, "graphs/" + filename, stateVertexWriter, edgeVertexWriter);
}
