#pragma once
#include <string>
#include <unordered_map>
#include "property.hpp"

using namespace std;

namespace graphdb
{
    struct Edge
    {
        string from;
        string to;
        double weight;
        PropertyMap poperties;

        void print() const;
    };
}
