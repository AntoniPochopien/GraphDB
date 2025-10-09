#include "storage.hpp"
#include "node.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;
using namespace graphdb;

namespace fs = filesystem;

const string Storage::BASE_PATH = "data/nodes";

// Constructor (Box init)
Storage::Storage(const string &box) : boxName(box)
{
    fs::path folderPath = BASE_PATH + "/" + boxName;
    if (!exists(folderPath))
    {
        create_directories(folderPath);
        cout << "Created new box: " << folderPath << "\n";
    }

    for (const auto &entry : fs::directory_iterator(folderPath))
    {
        auto name = entry.path().filename().string();
        if (name.find("nodes_") == 0 && name.find(".bin") != string::npos)
        {
            size_t idx = stoi(name.substr(6, name.size() - 10));
            if (idx >= lastChunkIdx)
                lastChunkIdx = idx + 1;
        }
    }
}

// ====================== SAVE NODE CHUNK ======================
void Storage::saveNodeChunk(const vector<Node> &nodes)
{
    string filename = "nodes_" + to_string(lastChunkIdx++) + ".bin";
    fs::path filepath = fs::path(BASE_PATH) / boxName / filename;

    ofstream out(filepath, ios::binary);
    if (!out)
    {
        cerr << "Cannot open file for writing: " << filepath << "\n";
        return;
    }

    size_t nodeCount = nodes.size();
    out.write(reinterpret_cast<const char *>(&nodeCount), sizeof(nodeCount));

    for (const auto &node : nodes)
    {
        size_t len = node.id.size();
        out.write(reinterpret_cast<const char *>(&len), sizeof(len));
        out.write(node.id.c_str(), len);

        size_t propCount = node.properties.size();
        out.write(reinterpret_cast<const char *>(&propCount), sizeof(propCount));

        for (const auto &[key, value] : node.properties)
        {
            size_t klen = key.size();
            out.write(reinterpret_cast<const char *>(&klen), sizeof(klen));
            out.write(key.c_str(), klen);

            value.serialize(out);
        }
    }

    out.close();
    cout << "Saved " << nodeCount << " nodes to " << filepath << "\n";
}

// ====================== LOAD NODE BY ID ======================
Node Storage::loadNodeById(const string &nodeId)
{
    auto it = nodeIndex.find(nodeId);
    if (it == nodeIndex.end())
    {
        throw runtime_error("NodeID not found in index: " + nodeId);
    }

    const string &file = it->second.first;
    size_t offset = it->second.second;

    ifstream in(file, ios::binary);
    if (!in)
    {
        throw runtime_error("Cannot open file: " + file);
    }

    in.seekg(offset);

    size_t idLen;
    in.read(reinterpret_cast<char *>(&idLen), sizeof(idLen));
    string id(idLen, '\0');
    in.read(&id[0], idLen);

    size_t propCount;
    in.read(reinterpret_cast<char *>(&propCount), sizeof(propCount));

    PropertyMap properties;
    for (size_t i = 0; i < propCount; ++i)
    {
        size_t klen;
        in.read(reinterpret_cast<char *>(&klen), sizeof(klen));
        string key(klen, '\0');
        in.read(&key[0], klen);

        PropertyValue val = PropertyValue::deserialize(in);
        properties[key] = val;
    }

    in.close();

    Node node;
    node.id = id;
    node.properties = properties;

    return node;
}

// ====================== BUILD NODE INDEX ======================
void Storage::buildNodeIndex()
{
    nodeIndex.clear();

    fs::path folder = fs::path(BASE_PATH) / boxName;

    if (!exists(folder))
    {
        cerr << "Folder does not exist: " << folder << "\n";
        return;
    }

    for (const auto &entry : fs::directory_iterator(folder))
    {
        if (entry.path().extension() != ".bin")
            continue;

        ifstream in(entry.path(), ios::binary);
        if (!in)
        {
            cerr << "Cannot open file: " << entry.path() << "\n";
            continue;
        }

        size_t nodeCount;
        in.read(reinterpret_cast<char *>(&nodeCount), sizeof(nodeCount));

        size_t offset = sizeof(nodeCount);

        for (size_t i = 0; i < nodeCount; ++i)
        {
            size_t idLen;
            size_t nodeStartOffset = offset; // <-- początek węzła
            in.read(reinterpret_cast<char *>(&idLen), sizeof(idLen));

            string id(idLen, '\0');
            in.read(&id[0], idLen);

            size_t propCount;
            in.read(reinterpret_cast<char *>(&propCount), sizeof(propCount));

            // przejdź po wszystkich właściwościach
            for (size_t j = 0; j < propCount; ++j)
            {
                size_t keyLen;
                in.read(reinterpret_cast<char *>(&keyLen), sizeof(keyLen));
                in.seekg(keyLen, ios::cur); // pomijamy klucz

                PropertyValue val = PropertyValue::deserialize(in); // odczytujemy wartość, ale nic z nią nie robimy
            }

            // zapis do indeksu: ID -> początek węzła
            nodeIndex[id] = {entry.path().string(), nodeStartOffset};

            // ustaw offset na początek następnego węzła
            offset = in.tellg();
        }

        in.close();
    }

    cout << "Built node index for " << nodeIndex.size() << " NodeIDs\n";
}
