#pragma once
#include <string>
#include <unordered_map>

using namespace std;

struct Edge {
    string from; 
    string to;
    double weight;
    unordered_map<string, string> properties;

    void print() const;
};