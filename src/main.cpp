#include "storage.hpp"
#include "graph.hpp"

using namespace graphdb;

int main(){
    //Test database flow
    Graph g;

    g.addNode(Node{"A", {{"color","red"}, {"value","10"}}});
    g.addNode(Node{"B", {{"color","blue"}}});

    Storage storage = Storage("box_name");

    storage.saveNodeChunk(g.getAllNodes());

    storage.buildNodeIndex();

    try {
        Node loadedNode = storage.loadNodeById("A");
        cout << "\nLoaded node A from storage:\n";
        loadedNode.print();
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}