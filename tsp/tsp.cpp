#include <fstream>
#include <vector>
#include <iostream>
#include <unordered_set>

const int PEEPHOLE_OPT_ITERATIONS = 10;
const double EPS = 1e-9;

struct Point {
    int id;
    double x, y;
};

struct Edge {
    int u, v;
    double weight;

    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

double EuclideanDistance(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}


class DSU {
public:
    DSU(int set_count)
        : Parent_(set_count)
        , Rank_(set_count)
    {
        for (int i = 0; i < set_count; i++) {
            Parent_[i] = i;
        }
    }

    int GetParent(int v) {
        if (Parent_[v] != v) {
            Parent_[v] = GetParent(Parent_[v]);
        }

        return Parent_[v];
    }

    bool Unite(int v, int u) {
        v = GetParent(v);
        u = GetParent(u);
        if (v == u) {
            return false;
        }

        if (Rank_[v] < Rank_[u]) {
            std::swap(v, u);
        }

        Parent_[u] = v;
        if (Rank_[v] == Rank_[u]) {
            Rank_[v]++;
        }

        return true;
    }

private:
    std::vector<int> Parent_;
    std::vector<int> Rank_;
};

std::vector<Edge> BuildMST(const std::vector<Point>& points) {
    std::vector<Edge> allEdges;
    for (int v = 0; v < (int)points.size(); v++) {
        for (int u = v + 1; u < (int)points.size(); u++) {
            allEdges.push_back({v, u, EuclideanDistance(points[v], points[u]) });
        }
    }

    std::sort(allEdges.begin(), allEdges.end());

    DSU dsu(points.size());
    std::vector<Edge> mst;

    for (const auto& edge : allEdges) {
        if (dsu.Unite(edge.u, edge.v)) {
            mst.push_back(edge);
            if (mst.size() + 1 == points.size()) {
                break;
            }
        }
    }

    return mst;
}

std::vector<int> FindOddDegreeVertices(const std::vector<Edge>& mst, int verticesCount) {
    std::vector<int> degree(verticesCount);

    for (const auto& edge : mst) {
        degree[edge.u]++;
        degree[edge.v]++;
    }

    std::vector<int> oddVertices;
    for (int v = 0; v < verticesCount; v++) {
        if (degree[v] % 2 == 1) {
            oddVertices.push_back(v);
        }
    }

    return oddVertices;
}

std::vector<Edge> GreedyMinimumMatching(const std::vector<int>& oddVertices, const std::vector<Point>& points) {
    std::vector<Edge> allEdges;
    for (int v = 0; v < (int)oddVertices.size(); v++) {
        for (int u = v + 1; u < (int)oddVertices.size(); u++) {
            allEdges.push_back({
                oddVertices[v], 
                oddVertices[u], EuclideanDistance(points[oddVertices[v]], points[oddVertices[u]])
            });
        }
    }

    std::sort(allEdges.begin(), allEdges.end());

    std::unordered_set<int> usedVertices;
    std::vector<Edge> matching;
    for (const auto& edge : allEdges) {
        if (usedVertices.find(edge.u) == usedVertices.end() && usedVertices.find(edge.v) == usedVertices.end()) {
            matching.push_back(edge);
            usedVertices.insert(edge.u);
            usedVertices.insert(edge.v);
        }
    }

    return matching;
}

std::vector<std::vector<int>> BuildAdjacencyList(const std::vector<Edge>& edges, int n) {
    std::vector<std::vector<int>> graph(n);
    for (const auto& edge : edges) {
        graph[edge.u].push_back(edge.v);
        graph[edge.v].push_back(edge.u);
    }

    return graph;
}

std::vector<int> FindEulerianCycle(std::vector<std::vector<int>> graph, int start) {
    std::vector<int> cycle;
    std::vector<int> stack;
    stack.push_back(start);

    while (!stack.empty()) {
        int v = stack.back();        
        if (!graph[v].empty()) {
            int u = graph[v].back();
            graph[v].pop_back();

            auto it = std::find(graph[u].begin(), graph[u].end(), v);
            if (it != graph[u].end()) {
                graph[u].erase(it);
            }

            stack.push_back(u);
        } else {
            cycle.push_back(v);
            stack.pop_back();
        }
    }

    std::reverse(cycle.begin(), cycle.end());
    return cycle;
}

std::vector<int> EulerianToHamiltonian(const std::vector<int>& eulerCycle) {
    std::vector<int> hamCycle;
    std::unordered_set<int> visited;
    for (int v : eulerCycle) {
        if (visited.find(v) == visited.end()) {
            hamCycle.push_back(v);
            visited.insert(v);
        }
    }

    return hamCycle;
}

double CalculateCycleLength(const std::vector<int>& cycle, const std::vector<Point>& points) {
    double length = 0.0;
    for (int v = 0; v < (int)cycle.size(); v++) {
        int u = (v + 1) % cycle.size();
        length += EuclideanDistance(points[cycle[v]], points[cycle[u]]);
    }

    return length;
}

// Optimize local intersections
void Peephole(std::vector<int>& cycle, const std::vector<Point>& points) {
    bool improved = true;
    for (int iteration = 0; improved && iteration < PEEPHOLE_OPT_ITERATIONS; iteration++) {
        improved = false;
        for (int i = 0; i < (int)cycle.size() - 1; i++) {
            for (int j = i + 2; j < (int)cycle.size(); j++) {
                if (j + 1 == cycle.size() && i == 0) {
                    continue; 
                }

                int i1 = cycle[i];
                int i2 = cycle[i + 1];
                int j1 = cycle[j];
                int j2 = cycle[(j + 1) % cycle.size()];

                double currentDist = EuclideanDistance(points[i1], points[i2]) + 
                                     EuclideanDistance(points[j1], points[j2]);
                double newDist = EuclideanDistance(points[i1], points[j1]) + 
                                 EuclideanDistance(points[i2], points[j2]);

                if (newDist < currentDist - EPS) {
                    std::reverse(cycle.begin() + i + 1, cycle.begin() + j + 1);
                    improved = true;
                }
            }
        }
    }
}

std::vector<int> ChristofidesAlgorithm(const std::vector<Point>& points) {    
    std::cout << "Running base algorithm...\n";

    auto mst = BuildMST(points);

    auto oddVertices = FindOddDegreeVertices(mst, points.size());
    auto matching = GreedyMinimumMatching(oddVertices, points);

    auto graph = BuildAdjacencyList(std::move(mst), points.size());
    auto eulerCycle = FindEulerianCycle(graph, 0);
    auto hamCycle = EulerianToHamiltonian(eulerCycle);

    std::cout << "Initial Hamiltonian cycle length: " << CalculateCycleLength(hamCycle, points) << '\n';
    std::cout << "Optimizing with 2-opt...\n";
    Peephole(hamCycle, points);
    std::cout << "After 2-opt: " << CalculateCycleLength(hamCycle, points) << '\n';

    return hamCycle;
}

int main() {
    std::ifstream fin("input.txt");
    std::vector<Point> points;

    int id = 0;
    double x = 0;
    double y = 0;
    while (fin >> id >> x >> y) {
        points.push_back({id - 1, x, y});
    }
    fin.close();
    std::cout << "Graph: " << points.size() << " vertices\n\n";

    auto cycle = ChristofidesAlgorithm(points);

    std::ofstream fout("output.txt");
    for (int v : cycle) {
        fout << v + 1 << ' ';
    }
    fout << '\n';
    fout.close();

    return 0;
}
