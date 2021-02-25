#ifndef REGEX_AST_HPP
#define REGEX_AST_HPP

#include "Token.hpp"
#include <vector>

struct Node{
    Token token;
    Node* leftChild = nullptr;
    Node* rightChild = nullptr;
};

struct AST{
    Node *topNode = nullptr;
};

#endif