#ifndef REGEX_NFA_HPP
#define REGEX_NFA_HPP

#include <map>
#include <set>
#include <stddef.h>
#include <vector>

struct FANode;

struct Edge{
    FANode *destination;
    char lable;

    Edge(){}

    Edge(FANode *node, char c)
    {
        destination = node;
        lable = c;
    }
};

struct FANode{
    unsigned char state;
    bool isAcceptState = false;
    std::vector<Edge> edges;
};

using NFATransactionTable = std::map<unsigned char, std::map<char, std::set<unsigned char>>>;

struct NFA{
    std::set<char> alphabet;
    std::map<unsigned char, FANode*> stateSet;
    unsigned char initalState;
    FANode *start;
    FANode *accept; 
};

struct DFA{
    std::set<char> alphabet;
    std::map<unsigned char, FANode*> stateSet;
    unsigned char initalState;
    FANode *start;
    FANode *accept; 
};

#endif