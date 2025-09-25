#include "graph.hpp"
#include "node.hpp"
#include "graph.hpp"
#include <iostream>

int main(){
      Graph g;

    // --- Dodanie węzłów ---
    g.addNode(Node{"A", {{"color","red"}, {"value","10"}}});
    g.addNode(Node{"B", {{"color","blue"}}});
    g.addNode(Node{"C", {{"color","green"}}});
    g.addNode(Node{"D", {{"color","yellow"}}});

    // --- Dodanie "dłuższych" edge z wieloma właściwościami ---
    g.addEdge(Edge{"A", "B", 1.0, {{"type","friend"}, {"weight_category","high"}}});
    g.addEdge(Edge{"A", "C", 2.5, {{"type","colleague"}, {"weight_category","medium"}}});
    g.addEdge(Edge{"B", "D", 0.5, {{"type","family"}, {"weight_category","low"}}});
    g.addEdge(Edge{"C", "D", 3.0, {{"type","friend"}, {"weight_category","high"}}});

    // --- Pobranie pojedynczego węzła ---
    auto n = g.getNode("A");
    if(n){
        cout << "Node A properties:\n";
        n->print();
    } else {
        cout << "Node A not found\n";
    }

    // --- Pobranie sąsiadów (neighbors) ---
    auto neighbors = g.getNeighbors("A"); // zwraca vector<Edge>
    cout << "\nNeighbors of A:\n";
    for(const auto& e : neighbors){
        cout << "A -> " << e.to << " (weight=" << e.weight << ")\n";
        for(const auto& prop : e.properties){
            cout << "   " << prop.first << " : " << prop.second << "\n";
        }
    }

    // --- Przykład filtrowania ---
    cout << "\nEdges from A with weight > 1.0:\n";
    for(const auto& e : neighbors){
        if(e.weight > 1.0){
            cout << "A -> " << e.to << " (weight=" << e.weight << ")\n";
        }
    }

    // --- Zwrot kilku obiektów w formie vector<Node> ---
    auto nodes = g.getNodesPage(1, 3); // np. pierwsze 3 węzły
    cout << "\nFirst 3 nodes in graph:\n";
    for(const auto& node : nodes){
        node.print();
    }

    return 0;
}