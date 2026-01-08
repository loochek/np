#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>

const int OPTIMIZE_MAX_ITERATIONS = 10;

std::vector<bool> EdgeBasedApproximation(const std::vector<std::pair<int, int>>& edges, int vertexCount) {
    std::vector<bool> vertexCoverMask(vertexCount);
    std::vector<bool> edgeCoverMask(edges.size());

    for (int i = 0; i < (int)edges.size(); i++) {
        if (!edgeCoverMask[i]) {
            // Pick up edge in greedy way
            auto [v , u] = edges[i];
            if (!vertexCoverMask[v]) {
                vertexCoverMask[v] = true;
            }
            if (!vertexCoverMask[u]) {
                vertexCoverMask[u] = true;
            }

            for (int j = 0; j < (int)edges.size(); j++) {
                auto [v2, u2] = edges[j];
                if (v2 == v || u2 == v || v2 == u || u2 == u) {
                    edgeCoverMask[j] = true;
                }
            }
        }
    }

    return vertexCoverMask;
}

int GetCoverSize(const std::vector<bool>& vertexCoverMask) {
    int size = 0;
    for (int i = 0; i < (int)vertexCoverMask.size(); i++) {
        if (vertexCoverMask[i]) {
            size++;
        }
    }
    return size;
}

bool IsVertexCover(const std::vector<bool>& vertexCoverMask, const std::vector<std::pair<int, int>>& edges) {
    bool allEdgesCovered = true;
    for (const auto& edge : edges) {
        if (!vertexCoverMask[edge.first] && !vertexCoverMask[edge.second]) {
            allEdgesCovered = false;
            break;
        }
    }
    return allEdgesCovered;
}

void RemoveRedundantVertices(std::vector<bool>& vertexCoverMask, const std::vector<std::pair<int, int>>& edges) {
    bool improved = true;
    for (int iteration = 0; iteration < OPTIMIZE_MAX_ITERATIONS && improved; iteration++) {
        improved = false;

        for (int v = 0; v < (int)vertexCoverMask.size(); v++) {
            if (!vertexCoverMask[v]) {
                continue;
            }

            vertexCoverMask[v] = false;
            if (!IsVertexCover(vertexCoverMask, edges)) {
                // Can't remove v, restore it
                vertexCoverMask[v] = true;
            } else {
                // Still a vertex cover
                improved = true;
            }
        }

        std::cout << "Optimize iteration " << iteration << ": vertex cover size = " << GetCoverSize(vertexCoverMask) << '\n';
    }
}

int main() {
    std::ifstream fin("input.txt");
    std::vector<std::pair<int, int>> edges;

    int verticesCount = 0;
    int u = 0, v = 0;
    while (fin >> u >> v) {
        verticesCount = std::max(verticesCount, std::max(u, v));
        u--; v--;
        edges.push_back({u, v});
    }
    fin.close();

    std::cout << "Graph: " << verticesCount << " vertices, " << edges.size() << " edges" << '\n';

    std::cout << "Running base algorithm...\n";
    auto vertexCoverMask = EdgeBasedApproximation(edges, verticesCount);
    std::cout << "Cover size found by base algorithm: " << GetCoverSize(vertexCoverMask) << '\n';
    
    std::cout << "\nOptimizing by removing redundant vertices..." << '\n';
    RemoveRedundantVertices(vertexCoverMask, edges);

    std::ofstream fout("output.txt");
    for (int i = 0; i < verticesCount; i++) {
        if (vertexCoverMask[i]) {
            fout << i + 1 << ' ';
        }
    }
    fout << '\n';
    fout.close();

    return 0;
}
