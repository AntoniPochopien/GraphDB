#pragma once
#include <any>
#include <string>
#include "property.hpp"

using namespace std;
using namespace graphdb;

namespace graphdb
{
    struct Node
    {
        string id;
        PropertyMap properties;

        void print() const;
    };
}