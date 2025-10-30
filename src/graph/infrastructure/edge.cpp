#include "edge.hpp"
#include <iostream>

using namespace std;
using namespace graphdb;

void Edge::print() const {
    cout << "Edge: " << from << " -> " << to << " (weight: " << weight << ")\n";
    for (const auto& [k, v] : properties)
        cout << "  " << k << ": (property)\n";
}

void Edge::serialize(ostream& out) const {
    size_t fromLen = from.size();
    out.write(reinterpret_cast<const char*>(&fromLen), sizeof(fromLen));
    out.write(from.data(), fromLen);

    size_t toLen = to.size();
    out.write(reinterpret_cast<const char*>(&toLen), sizeof(toLen));
    out.write(to.data(), toLen);

    out.write(reinterpret_cast<const char*>(&weight), sizeof(weight));

    size_t propCount = properties.size();
    out.write(reinterpret_cast<const char*>(&propCount), sizeof(propCount));
    for (const auto& [k, v] : properties) {
        size_t klen = k.size();
        out.write(reinterpret_cast<const char*>(&klen), sizeof(klen));
        out.write(k.data(), klen);
        v.serialize(out);
    }
}

Edge Edge::deserialize(istream& in) {
    Edge edge;

    size_t fromLen;
    in.read(reinterpret_cast<char*>(&fromLen), sizeof(fromLen));
    edge.from.resize(fromLen);
    in.read(edge.from.data(), fromLen);

    size_t toLen;
    in.read(reinterpret_cast<char*>(&toLen), sizeof(toLen));
    edge.to.resize(toLen);
    in.read(edge.to.data(), toLen);

    in.read(reinterpret_cast<char*>(&edge.weight), sizeof(edge.weight));

    size_t propCount;
    in.read(reinterpret_cast<char*>(&propCount), sizeof(propCount));

    for (size_t i = 0; i < propCount; ++i) {
        size_t klen;
        in.read(reinterpret_cast<char*>(&klen), sizeof(klen));
        string key(klen, '\0');
        in.read(key.data(), klen);

        PropertyValue val = PropertyValue::deserialize(in);
        edge.properties.emplace(std::move(key), std::move(val));
    }

    return edge;
}
