#pragma once
#include <string>
#include <unordered_map>

using namespace std;

struct Node {
    string id;
    unordered_map<string, string> properties;

    void print() const;
};