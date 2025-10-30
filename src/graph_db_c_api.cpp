#include "graph_db_c_api.h"
#include "storage.hpp"
#include <string>
#include <memory>

using namespace graphdb;
using namespace std;
using json = nlohmann::json;

struct GraphDB {
    unique_ptr<Storage> storage;
};

extern "C" {

    // Initialize storage
    GraphDB* graphdb_init(const char* boxName) {
        if (!boxName) return nullptr;

        auto* box = new GraphDB();
        box->storage = make_unique<Storage>(string(boxName));
        box->storage->buildNodeIndex();
        box->storage->buildEdgeIndex();

        return box;
    }

    // Save nodes from JSON
    void graphdb_save_nodes(GraphDB* box, const char* jsonData) {
        if (!box || !jsonData) return;

        // deserialize JSON → vector<Node>
        vector<Node> nodes = parse_nodes_from_json(jsonData);
        box->storage->saveNodeChunk(nodes);
    }

    vector<Node> parse_nodes_from_json(const char* jsonData) {
    vector<Node> nodes;
    if (!jsonData) return nodes;

    json j = json::parse(jsonData);
    if (!j.is_array()) throw runtime_error("Expected JSON array of nodes");

    for (const auto& item : j) {
        // convert each JSON object to Node
        string nodeStr = item.dump();
        Node n = Node::from_json(nodeStr);
        nodes.push_back(move(n));
    }

    return nodes;
}

    // Load node by ID
    const char* graphdb_load_node(GraphDB* box, const char* nodeId) {
        if (!box || !nodeId) return nullptr;

        try {
            Node node = box->storage->loadNodeById(nodeId);
            string json = node.to_json();
            char* result = (char*)malloc(json.size() + 1);
            strcpy(result, json.c_str());
            return result;
        } catch (...) {
            return nullptr;
        }
    }

    // Free C string
    void graphdb_free_string(const char* str) {
        free((void*)str);
    }

    // Clean up
    void graphdb_close(GraphDB* box) {
        delete box;
    }

} // extern "C"
