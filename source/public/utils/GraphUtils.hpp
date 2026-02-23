#pragma once

#include <automaton/NFA.hpp>
#include <automaton/Edge.hpp>
#include <graaflib/graph.h>

struct GraphVertex {
    State state;
    bool isStart;
    bool isAccept;
};

class NFAGraph {
public:
    explicit NFAGraph(const std::shared_ptr<NFA>& nfa);

    void printGraph(const std::string& filename = "graph") const;

private:
    graaf::directed_graph<GraphVertex, Edge> graph;

};

const auto stateVertexWriter{
    [](graaf::vertex_id_t vertex_id, const GraphVertex& vertex) -> std::string {
        return "label=\"" + static_cast<std::string>(vertex.isStart ? "Start : " : vertex.isAccept ? "Accept : " : "")
            + std::to_string(vertex.state) + "\""
            + (vertex.isStart || vertex.isAccept ? "style=\"bold\"" : "");
    }};

const auto edgeVertexWriter{
    [](const graaf::edge_id_t& /*edge_id*/, const Edge& edge) -> std::string {
        return edge.isEpsilon()? "" : "label=\"" + edge.getLabel() + "\"";
    }};
