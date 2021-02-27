#ifndef REGEX_AST_HPP
#define REGEX_AST_HPP

#include <CommonDataStructure/Token.hpp>
#include <vector>

struct ASTNode{
    Token token;
    ASTNode* leftChild = nullptr;
    ASTNode* rightChild = nullptr;

    ASTNode() = default;

    ASTNode(const Token &t)
    {
        token = t;
    }
};

struct AST{
    ASTNode*topNode = nullptr;
    bool isVaild = false;
    std::string errorMsg;

    ~AST()
    {
        if(topNode != nullptr){
            cleanTree(topNode);
        }
    }

    void cleanTree(ASTNode *node)
    {
        if(node->leftChild != nullptr){
            cleanTree(node->leftChild);
        }
        if(node->rightChild != nullptr){
            cleanTree(node->rightChild);
        }
        delete node;
    }
};

#endif