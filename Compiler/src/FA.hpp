#ifndef REGEX_NFA_HPP
#define REGEX_NFA_HPP

#include <map>
#include <set>
#include <stddef.h>
#include <vector>

struct FANode;

struct FASymbol{
    char lowerBound;
    char upperBound;
};

inline bool operator<(const FASymbol &lhs, const FASymbol &rhs)
{
    if(lhs.lowerBound < rhs.lowerBound){
        return true;
    } else if(lhs.lowerBound == rhs.lowerBound){
        if(lhs.upperBound < rhs.upperBound){
            return true;
        }else{
            return false;
        }
    } else{
        return false;
    }
}

inline bool operator==(const FASymbol &lhs, const FASymbol &rhs)
{
    return lhs.lowerBound == rhs.lowerBound && lhs.upperBound == rhs.upperBound;
}

struct Edge{
    FANode *destination;
    FASymbol lable;

    Edge(){}

    Edge(FANode *dest, char c)
    {
        destination = dest;
        lable.upperBound = lable.lowerBound = c;
    }

    Edge(FANode *dest, FASymbol symbol)
    {
        destination = dest;
        lable = symbol;
    }
};

using FAState = unsigned char;

struct FANode{ 
    FAState state;
    bool isAcceptState = false;
    std::vector<Edge> edges;
};


struct NFA{
    std::set<FASymbol> alphabet;
    std::map<unsigned char, FANode*> stateSet;
    FANode *start;
    FANode *accept; 

    ~NFA()
    {
        for (auto state : stateSet)
        {
            delete state.second;
        }
        stateSet.clear();
        start = accept = nullptr;
    }
};

struct DFA  {
    std::map<unsigned char, FANode*> stateSet;
    FANode *start;
    FANode *accept; 

    ~DFA  ()
    {
        for (auto state : stateSet)
        {
            delete state.second;
        }
        stateSet.clear();
        start = accept = nullptr;
    }
};

#endif