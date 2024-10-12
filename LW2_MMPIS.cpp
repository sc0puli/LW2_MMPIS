#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <climits>
#include <string>
#include <limits>

using namespace std;

// Структура для представления вершины
struct Vertex {
    string name;
    double delay;
    double earliestTime = 0.0;
    double latestTime = numeric_limits<double>::infinity();
};

// Структура для представления графа
class Graph {
public:
    unordered_map<string, Vertex> vertices;
    unordered_map<string, vector<pair<string, double>>> adjList;  // список смежности с весами
    unordered_map<string, vector<pair<string, double>>> reverseAdjList; // обратный список смежности

    void addVertex(const string& name, double delay) {
        vertices[name] = { name, delay };
    }

    void addEdge(const string& source, const string& dest, double weight) {
        adjList[source].emplace_back(dest, weight);
        reverseAdjList[dest].emplace_back(source, weight);
    }

    void topologicalSortUtil(const string& v, unordered_map<string, bool>& visited, vector<string>& stack) {
        visited[v] = true;

        for (auto& neighbor : adjList[v]) {
            if (!visited[neighbor.first]) {
                topologicalSortUtil(neighbor.first, visited, stack);
            }
        }
        stack.push_back(v);
    }

    vector<string> topologicalSort() {
        unordered_map<string, bool> visited;
        vector<string> stack;

        for (auto& pair : vertices) {
            if (!visited[pair.first]) {
                topologicalSortUtil(pair.first, visited, stack);
            }
        }

        reverse(stack.begin(), stack.end());
        return stack;
    }

    void calculateEarliestTimes(const vector<string>& topoOrder) {
        for (const auto& v : topoOrder) {
            for (const auto& neighbor : adjList[v]) {
                string dest = neighbor.first;
                double edgeWeight = neighbor.second;
                vertices[dest].earliestTime = max(vertices[dest].earliestTime,
                    vertices[v].earliestTime + vertices[v].delay + edgeWeight);
            }
        }
    }

    void calculateLatestTimes(const vector<string>& topoOrder, double requiredTime) {
        vertices[topoOrder.back()].latestTime = requiredTime;

        for (auto it = topoOrder.rbegin(); it != topoOrder.rend(); ++it) {
            const string& v = *it;
            for (const auto& neighbor : adjList[v]) {
                string dest = neighbor.first;
                double edgeWeight = neighbor.second;
                vertices[v].latestTime = min(vertices[v].latestTime,
                    vertices[dest].latestTime - vertices[v].delay - edgeWeight);
            }
        }
    }

    void findViolations(ofstream& outputFile) {
        for (const auto& pair : vertices) {
            const Vertex& vertex = pair.second;
            double slack = vertex.latestTime - vertex.earliestTime;
            /*outputFile << "Vertex " << vertex.name
                << " -> Earliest: " << vertex.earliestTime
                << ", Latest: " << vertex.latestTime
                << ", Slack: " << slack << endl;*/
            if (slack < 0) {
                outputFile << "Vertex " << vertex.name << " violates timing with slack: " << slack << endl;
            }
        }
    }
};

int main() {
    // Открытие файлов
    ifstream inputFile("input.txt");
    ofstream outputFile("output.txt");

    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << "Error: Unable to open input/output file!" << endl;
        return 1;
    }

    int V, E;
    double requiredTime;
    inputFile >> V >> E >> requiredTime;

    Graph g;

    for (int i = 0; i < V; ++i) {
        string name;
        double delay;
        inputFile >> name >> delay;
        g.addVertex(name, delay);
    }

    for (int i = 0; i < E; ++i) {
        string source, dest;
        double weight;
        inputFile >> source >> dest >> weight;
        g.addEdge(source, dest, weight);
    }

    vector<string> topoOrder = g.topologicalSort();
    g.calculateEarliestTimes(topoOrder);
    g.calculateLatestTimes(topoOrder, requiredTime);
    g.findViolations(outputFile);

    // Закрытие файлов
    inputFile.close();
    outputFile.close();

    return 0;
}
