#include "node.hpp"
#include <iostream>

using namespace std;
using namespace graphdb;

void Node::print() const {
    cout << "Node id: " << id << "\n";
    for (const auto& [k, v] : properties)
        cout << "  " << k << ": (property)\n";
}

void Node::serialize(ostream& out) const {
    size_t idLen = id.size();
    out.write(reinterpret_cast<const char*>(&idLen), sizeof(idLen));
    out.write(id.data(), idLen);

    size_t propCount = properties.size();
    out.write(reinterpret_cast<const char*>(&propCount), sizeof(propCount));

    for (const auto& [k, v] : properties) {
        size_t klen = k.size();
        out.write(reinterpret_cast<const char*>(&klen), sizeof(klen));
        out.write(k.data(), klen);
        v.serialize(out);
    }
}

Node Node::deserialize(istream& in) {
    Node node;

    size_t idLen;
    in.read(reinterpret_cast<char*>(&idLen), sizeof(idLen));
    node.id.resize(idLen);
    in.read(node.id.data(), idLen);

    size_t propCount;
    in.read(reinterpret_cast<char*>(&propCount), sizeof(propCount));

    for (size_t i = 0; i < propCount; ++i) {
        size_t klen;
        in.read(reinterpret_cast<char*>(&klen), sizeof(klen));
        string key(klen, '\0');
        in.read(key.data(), klen);

        PropertyValue val = PropertyValue::deserialize(in);
        node.properties.emplace(std::move(key), std::move(val));
    }

    return node;
}
