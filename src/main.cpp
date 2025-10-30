#include "storage.hpp"
#include "graph.hpp"

using namespace graphdb;

int main()
{
    // --- Tworzymy graf w pamięci ---
    Graph g;

    g.addNode(Node{"Alice", {{"age", "25"}, {"city", "Paris"}}});
    g.addNode(Node{"Bob", {{"age", "30"}, {"city", "Berlin"}}});
    g.addNode(Node{"Carol", {{"age", "22"}, {"city", "London"}}});
    g.addNode(Node{"Dave", {{"age", "28"}, {"city", "Madrid"}}});

    g.addEdge(Edge{"Alice", "Bob", 1.0});
    g.addEdge(Edge{"Alice", "Carol", 1.0});
    g.addEdge(Edge{"Bob", "Dave", 1.0});
    g.addEdge(Edge{"Carol", "Dave", 1.0});

    // --- Storage ---
    Storage storage("users_box");

    storage.buildNodeIndex();
    storage.buildEdgeIndex();

    // --- Zapis nowych danych ---
    storage.saveNodeChunk(g.getAllNodes());
    storage.saveEdgeChunk(g.getAllEdges());

    storage.buildNodeIndex();
    storage.buildEdgeIndex();

    try
    {
        Node loaded = storage.loadNodeById("Alice");
        cout << "\nLoaded node Alice:\n";
        loaded.print();

        // Pobieramy sąsiadów (edge'y wychodzące z Alice)
        auto neighbors = storage.loadEdgesFromNode("Alice");
        cout << "Alice's friends: ";
        for (const auto &e : neighbors)
            cout << e.to << " ";
        cout << "\n";
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}