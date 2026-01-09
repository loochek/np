#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

const int TWO_OPT_ITERATIONS = 10;

struct Edge {
    int from;
    int to;
    int weight;

    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

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


int CalculateOverlap(std::string_view s1, std::string_view s2) {
    for (int overlap = std::min(s1.size(), s2.size()); overlap > 0; overlap--) {
        if (s1.substr(s1.size() - overlap) == s2.substr(0, overlap)) {
            return overlap;
        }
    }

    return 0;
}

bool IsSubstring(const std::string& inner, const std::string& outer) {
    if (inner.size() > outer.size()) {
        return false;
    }

    return outer.find(inner) != std::string::npos;
}

std::vector<std::string> RemoveSubstrings(std::vector<std::string> strings) {
    std::sort(strings.begin(), strings.end(), [](const std::string& a, const std::string& b) {
        return a.size() > b.size();
    });

    std::vector<bool> substring(strings.size());
    for (int i = 0; i < strings.size(); i++) {
        if (substring[i]) {
            continue;
        }

        for (int j = i + 1; j < strings.size(); j++) {
            if (!substring[j] && IsSubstring(strings[j], strings[i])) {
                substring[j] = true;
            }
        }
    }

    std::vector<std::string> result;
    for (int i = 0; i < strings.size(); i++) {
        if (!substring[i]) {
            result.push_back(std::move(strings[i]));
        }
    }

    return result;
}

std::vector<std::vector<int>> BuildOverlapMatrix(const std::vector<std::string>& strings) {
    std::vector<std::vector<int>> overlap(strings.size());
    for (int i = 0; i < strings.size(); i++) {
        overlap[i].resize(strings.size());
        for (int j = 0; j < strings.size(); j++) {
            if (i != j) {
                overlap[i][j] = CalculateOverlap(strings[i], strings[j]);
            }
        }
    }

    return overlap;
}

int CalculateSuperstringLength(const std::vector<std::string>& strings,
                                const std::vector<int>& order,
                                const std::vector<std::vector<int>>& overlap) {
    if (order.empty()) {
        return 0;
    }

    int length = strings[order[0]].size();
    for (int i = 1; i < order.size(); i++) {
        int prev = order[i - 1];
        int curr = order[i];
        length += strings[curr].size() - overlap[prev][curr];
    }
    
    return length;
}

std::string BuildSuperstring(const std::vector<std::string>& strings,
                             const std::vector<int>& order,
                             const std::vector<std::vector<int>>& overlap) {
    if (order.empty()) {
        return "";
    }

    std::stringstream result;
    result << strings[order[0]];
    for (int i = 1; i < order.size(); i++) {
        int prev = order[i - 1];
        int curr = order[i];
        result << strings[curr].substr(overlap[prev][curr]);
    }

    return result.str();
}

std::vector<int> GreedySuperstring(const std::vector<std::string>& strings, const std::vector<std::vector<int>>& overlap) {
    if (strings.size() == 0) {
        return {};
    }
    if (strings.size() == 1) {
        return {0};
    }

    std::vector<int> next(strings.size(), -1);
    std::vector<int> prev(strings.size(), -1);
    
    DSU classes(strings.size());

    // Build full graph of edges with cost = overlap[from][to]
    std::vector<Edge> edges;
    for (int from = 0; from < strings.size(); from++) {
        for (int to = 0; to < strings.size(); to++) {
            if (from != to) {
                edges.push_back({from, to, overlap[from][to]});
            }
        }
    }

    // Unite strings into chains in greedy way
    std::sort(edges.begin(), edges.end());

    int edgesAdded = 0;
    for (const auto& e : edges) {
        if (edgesAdded + 1 >= strings.size()) {
            break;
        }

        if (next[e.from] == -1 && prev[e.to] == -1 && classes.GetParent(e.from) != classes.GetParent(e.to)) {
            classes.Unite(e.from, e.to);
            next[e.from] = e.to;
            prev[e.to] = e.from;
            edgesAdded++;
        }
    }

    // Merge chains

    std::vector<int> starts;
    for (int i = 0; i < strings.size(); i++) {
        if (prev[i] == -1) {
            starts.push_back(i);
        }
    }

    for (int i = 1; i < starts.size(); i++) {
        int end = starts[i - 1];
        while (next[end] != -1) {
            end = next[end];
        }
        next[end] = starts[i];
        prev[starts[i]] = end;
    }
    
    std::vector<int> order;
    for (int curr = starts[0]; curr != -1; curr = next[curr]) {
        order.push_back(curr);
    }

    return order;
}

void TwoOptOptimization(std::vector<int>& order,
                        const std::vector<std::string>& strings,
                        const std::vector<std::vector<int>>& overlap) {
    if (order.size() < 3) {
        return;
    }

    bool improved = true;
    for (int iteration = 0; improved && iteration < TWO_OPT_ITERATIONS; iteration++) {
        improved = false;

        int bestLength = CalculateSuperstringLength(strings, order, overlap);
        for (int i = 0; i + 1 < order.size() && !improved; i++) {
            for (int j = i + 2; j < order.size() && !improved; j++) {
                std::vector<int> newOrder = order;
                std::reverse(newOrder.begin() + i + 1, newOrder.begin() + j + 1);

                if (CalculateSuperstringLength(strings, newOrder, overlap) < bestLength) {
                    improved = true;
                    order = newOrder;
                }
            }
        }
    }
}

void SwapOptimization(std::vector<int>& order,
                       const std::vector<std::string>& strings,
                       const std::vector<std::vector<int>>& overlap) {
    if (order.size() < 3) {
        return;
    }

    bool improved = true;
    while (improved) {
        improved = false;

        int bestLength = CalculateSuperstringLength(strings, order, overlap);
        for (int i = 0; i < order.size() && !improved; i++) {
            std::vector<int> newOrder = order;
            newOrder.erase(newOrder.begin() + i);

            for (int j = 0; j <= newOrder.size() && !improved; j++) {
                std::vector<int> testOrder = newOrder;
                testOrder.insert(testOrder.begin() + j, order[i]);

                if (CalculateSuperstringLength(strings, testOrder, overlap) < bestLength) {
                    order = testOrder;
                    improved = true;
                }
            }
        }
    }
}

int main() {
    std::ifstream input("input.txt");
    std::vector<std::string> strings;

    std::string string;
    while (input >> string) {
        strings.push_back(std::move(string));
    }
    input.close();

    std::cout << "Input: " << strings.size() << " strings\n";

    std::cout << "Removing redundant substrings...\n";
    strings = RemoveSubstrings(strings);
    std::cout << "After removing substrings: " << strings.size() << " strings\n\n";

    auto overlap = BuildOverlapMatrix(strings);

    std::cout << "Running greedy algorithm...\n";
    auto order = GreedySuperstring(strings, overlap);
    int length = CalculateSuperstringLength(strings, order, overlap);
    std::cout << "After greedy: " << length << '\n';

    std::cout << "Running 2-opt optimization...\n";
    TwoOptOptimization(order, strings, overlap);
    length = CalculateSuperstringLength(strings, order, overlap);
    std::cout << "After 2-opt: " << length << '\n';

    std::cout << "Running swap optimization...\n";
    SwapOptimization(order, strings, overlap);
    length = CalculateSuperstringLength(strings, order, overlap);
    std::cout << "After swap: " << length << '\n';

    std::string superstring = BuildSuperstring(strings, order, overlap);
    std::cout << "\nFinal superstring length: " << superstring.size() << '\n';

    std::ofstream output("output.txt");
    output << superstring << '\n';
    output.close();

    return 0;
}
