#include <fstream>
#include <vector>
#include <iostream>
#include <unordered_set>

struct Point {
    int id;
    double x, y;
};

double EuclideanDistance(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

int main() {
    std::ifstream graphFile("input.txt");
    std::vector<Point> points;
    int v = 0;
    double x = 0.0;
    double y = 0.0;
    while (graphFile >> v >> x >> y) {
        points.push_back({v, x, y});
    }
    graphFile.close();

    std::cout << "Graph loaded: " << points.size() << " vertices\n";

    std::ifstream cycleFile("output.txt");
    std::vector<int> cycle;
    while (cycleFile >> v) {
        cycle.push_back(v);
    }
    cycleFile.close();

    std::cout << "Cycle size: " << cycle.size() << '\n';

    if (cycle.size() != points.size()) {
        std::cout << "ERROR: Cycle size (" << cycle.size() 
                  << ") does not match number of vertices (" << points.size() << ")\n";
        return 1;
    }

    std::unordered_set<int> visitedVertices;
    for (int vertex : cycle) {
        if (visitedVertices.count(vertex) > 0) {
            std::cout << "ERROR: Vertex " << vertex << " appears more than once in the cycle\n";
            return 1;
        }
        visitedVertices.insert(vertex);
    }

    for (int vertex : cycle) {
        if (vertex < 1 || vertex > (int)points.size()) {
            std::cout << "ERROR: Vertex " << vertex << " is out of range [1, " << points.size() << "]\n";
            return 1;
        }
    }

    std::cout << "Cycle is a valid Hamiltonian cycle\n";

    double totalLength = 0.0;
    for (int i = 0; i < (int)cycle.size(); i++) {
        int currentVertex = cycle[i] - 1;
        int nextVertex = cycle[(i + 1) % cycle.size()] - 1;

        double distance = EuclideanDistance(points[currentVertex], points[nextVertex]);
        totalLength += distance;
    }

    std::cout << "Cycle length: " << totalLength << '\n';
    return 0;
}
