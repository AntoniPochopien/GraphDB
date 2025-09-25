#include "edge.hpp"
#include <iostream>

using namespace std;


void Edge::print() const {
    cout << "Edge from: " << from << " to: " << to << " weight: " << weight << "\n";
}