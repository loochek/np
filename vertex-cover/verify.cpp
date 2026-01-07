#include <fstream>
#include <vector>
#include <iostream>
#include <unordered_set>

int main() {
    std::ifstream graphFile("input.txt");
    std::vector<std::pair<int, int>> edges;
    int maxVertex = 0;
    int u = 0;
    int v = 0;
    while (graphFile >> u >> v) {
        maxVertex = std::max(maxVertex, std::max(u, v));
        edges.push_back({u, v});
    }
    graphFile.close();

    std::cout << "Graph loaded: " << maxVertex << " vertices, " << edges.size() << " edges\n";

    std::ifstream coverFile("output.txt");
    std::unordered_set<int> vertexCover;
    while (coverFile >> v) {
        vertexCover.insert(v);
    }
    coverFile.close();

    std::cout << "Vertex cover size: " << vertexCover.size() << '\n';

    int uncoveredEdges = 0;    
    for (const auto& edge : edges) {
        bool covered = vertexCover.count(edge.first) > 0 || vertexCover.count(edge.second) > 0;
        if (!covered) {
            std::cout << "Edge (" << edge.first << ", " << edge.second << ") is not covered\n";
            uncoveredEdges++;
        }
    }

    if (uncoveredEdges > 0) {
        std::cout << "Vertex cover is INCORRECT\n";
        std::cout << "Total uncovered edges: " << uncoveredEdges << '\n';
    } else {
        std::cout << "Vertex cover is correct\n";
    }

    return 0;
}
