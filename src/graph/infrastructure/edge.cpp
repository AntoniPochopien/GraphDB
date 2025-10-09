#include "edge.hpp"
#include <iostream>

using namespace std;
using namespace graphdb;

void Edge::print() const
{
    cout << "Edge from: " << from << " to: " << to << " weight: " << weight << "\n";
}