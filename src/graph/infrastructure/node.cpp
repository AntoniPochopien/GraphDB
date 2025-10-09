#include "node.hpp"
#include <iostream>

using namespace std;
using namespace graphdb;

void Node::print() const {
    cout << "Node id: " << id <<"\n";
};