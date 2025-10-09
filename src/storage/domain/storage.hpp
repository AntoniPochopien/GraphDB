#pragma once
#include <string>
#include <utility>
#include <vector>
#include <filesystem>
#include "node.hpp"

using namespace std;
namespace fs = filesystem;


namespace graphdb
{
    class Storage
    {
    public:
        Storage(const string& box);
        ~Storage() = default;

        void saveNodeChunk(const vector<Node> &nodes);

        Node loadNodeById(const string &nodeId);

        void buildNodeIndex();

    private:
        string boxName;
        unordered_map<string, pair<string, size_t>> nodeIndex;
        int lastChunkIdx;
        
        static const string BASE_PATH;
    };
}