#ifndef REGEX_AST_HPP
#define REGEX_AST_HPP

#include "Token.hpp"
#include <vector>

struct Node{
    Token token;
    Node* leftChild;
    Node* rightChild;
};

struct AST{
    Node *topNode;
};

#endif