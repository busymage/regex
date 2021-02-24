#include <CommonDataStructure/AST.hpp>
#include <CommonDataStructure/FA.hpp>
#include <CommonDataStructure/Token.hpp>
#include <Compiler/NFABuilder.hpp>
#include <gtest/gtest.h>
#include <set>

TEST(NFABuilderTest, empty)
{
    NFABuilder builder;
    NFA *nfa = builder.fromAST(nullptr);
    ASSERT_TRUE(nfa != nullptr);
    ASSERT_EQ(2, nfa->stateSet.size());
    FANode *startNode = nfa->start;
    FANode *acceptNode = nfa->accept;
    ASSERT_EQ(1, startNode->edges.size());
    ASSERT_EQ('\0', startNode->edges[0].lable);
    ASSERT_EQ(acceptNode, startNode->edges[0].destination);
}

TEST(NFABuilderTest, singleCharacter)
{
    AST *ast = new AST;
    Node *node = new Node;
    node->leftChild = node->rightChild = nullptr;
    node->token = {TokenType::CHAR, "a"};
    ast->topNode = node;
    NFABuilder builder;
    NFA *nfa = builder.fromAST(ast);
    ASSERT_TRUE(nfa != nullptr);
    ASSERT_EQ(2, nfa->stateSet.size());
    FANode *startNode = nfa->start;
    FANode *acceptNode = nfa->accept;
    ASSERT_EQ(1, startNode->edges.size());
    ASSERT_EQ(acceptNode, startNode->edges[0].destination);

    std::set<char> inputAlphabet = nfa->alphabet;
    ASSERT_EQ(std::set<char>{'a'}, inputAlphabet);
}

TEST(NFABuilderTest, Union)
{
    // a|b
    AST *ast = new AST;
    Node *node = new Node;
    node->leftChild = node->rightChild = nullptr;
    node->token = {TokenType::OR, "|"};
    ast->topNode = node;

    Node *a = new Node;
    a->leftChild = a->rightChild = nullptr;
    a->token = {TokenType::CHAR, "a"};
    node->leftChild = a;

    Node *b = new Node;
    b->leftChild = b->rightChild = nullptr;
    b->token = {TokenType::OR, "b"};
    node->rightChild = b;
    
    NFABuilder builder;
    NFA *nfa = builder.fromAST(ast);
    ASSERT_TRUE(nfa != nullptr);
    ASSERT_EQ(6, nfa->stateSet.size());
    
    FANode *startNode = nfa->start;
    ASSERT_EQ(2, startNode->edges.size());
    for (size_t i = 0; i < 2; i++)
    {
        Edge edge = startNode->edges[i];
        ASSERT_NE(edge.destination, nfa->accept);
        ASSERT_EQ(edge.destination->edges.size(), 1);
        ASSERT_NE(edge.destination->edges[0].destination, nfa->accept);
        ASSERT_EQ(edge.destination->edges[0].destination->edges.size(), 1);
        ASSERT_EQ(edge.destination->edges[0].destination->edges[0].destination, nfa->accept);
    }

    std::set<char> inputAlphabet = nfa->alphabet;
    std::set<char> testSet;
    testSet.insert('a');
    testSet.insert('b');
    ASSERT_EQ(testSet, inputAlphabet);
}

TEST(NFABuilderTest, concatenation)
{
    // ab
    AST *ast = new AST;
    Node *node = new Node;
    node->leftChild = node->rightChild = nullptr;
    node->token = {TokenType::CAT, ""};
    ast->topNode = node;

    Node *a = new Node;
    a->leftChild = a->rightChild = nullptr;
    a->token = {TokenType::CHAR, "a"};
    node->leftChild = a;

    Node *b = new Node;
    b->leftChild = b->rightChild = nullptr;
    b->token = {TokenType::OR, "b"};
    node->rightChild = b;
    
    NFABuilder builder;
    NFA *nfa = builder.fromAST(ast);
    ASSERT_TRUE(nfa != nullptr);
    ASSERT_EQ(4, nfa->stateSet.size());
    
    FANode *startNode = nfa->start;
    ASSERT_EQ(1, startNode->edges.size());
    FANode *n2 = startNode->edges[0].destination;
    ASSERT_NE(n2, nullptr);
    ASSERT_NE(n2, nfa->accept);
    ASSERT_EQ(1, n2->edges.size());

    FANode *n3 = n2->edges[0].destination;
    ASSERT_NE(n3, nullptr);
    ASSERT_NE(n3, nfa->accept);
    ASSERT_EQ(1, n3->edges.size());
    
    FANode *n4 = n3->edges[0].destination;
    ASSERT_NE(n4, nullptr);
    ASSERT_EQ(n4, nfa->accept);
    ASSERT_EQ(0, n4->edges.size());

    std::set<char> inputAlphabet = nfa->alphabet;
    std::set<char> testSet;
    testSet.insert('a');
    testSet.insert('b');
    ASSERT_EQ(testSet, inputAlphabet);
}

TEST(NFABuilderTest, closure)
{
    // a*
    AST *ast = new AST;
    Node *node = new Node;
    node->leftChild = node->rightChild = nullptr;
    node->token = {TokenType::ZERO_MORE, "*"};
    ast->topNode = node;

    Node *a = new Node;
    a->leftChild = a->rightChild = nullptr;
    a->token = {TokenType::CHAR, "a"};
    node->leftChild = a;
    
    NFABuilder builder;
    NFA *nfa = builder.fromAST(ast);
    ASSERT_TRUE(nfa != nullptr);
    ASSERT_EQ(4, nfa->stateSet.size());
    
    //1
    FANode *startNode = nfa->start;
    ASSERT_EQ(2, startNode->edges.size());

    //2
    FANode *n2 = startNode->edges[0].destination;
    ASSERT_NE(n2, nullptr);
    ASSERT_NE(n2, nfa->accept);
    ASSERT_EQ(1, n2->edges.size());

    //4
    FANode *n4 = startNode->edges[1].destination;
    ASSERT_NE(n4, nullptr);
    ASSERT_EQ(n4, nfa->accept);
    ASSERT_EQ(0, n4->edges.size());

    //3
    FANode *n3 = n2->edges[0].destination;
    ASSERT_NE(n3, nullptr);
    ASSERT_NE(n3, nfa->accept);
    ASSERT_EQ(2, n3->edges.size());
    for (size_t i = 0; i < 2; i++)
    {
        FANode *n = n3->edges[i].destination;
        ASSERT_NE(n, startNode);
        ASSERT_NE(n, n3);
    }

    std::set<char> inputAlphabet = nfa->alphabet;
    ASSERT_EQ(std::set<char>{'a'}, inputAlphabet);
}

TEST(NFABuilderTest, aOrbGroupFollowByStar)
{
    //(a|b)*
    AST *ast = new AST;
    Node *node = new Node;
    node->leftChild = node->rightChild = nullptr;
    node->token = {TokenType::ZERO_MORE, "*"};
    ast->topNode = node;

    Node *or_ = new Node;
    or_->leftChild = or_->rightChild = nullptr;
    or_->token = {TokenType::OR, "|"};
    node->leftChild = or_;

    Node *a = new Node;
    a->leftChild = a->rightChild = nullptr;
    a->token = {TokenType::CHAR, "a"};
    or_->leftChild = a;

    Node *b = new Node;
    b->leftChild = b->rightChild = nullptr;
    b->token = {TokenType::CHAR, "b"};
    or_->rightChild = b;

    NFABuilder builder;
    NFA *nfa = builder.fromAST(ast);
    ASSERT_TRUE(nfa != nullptr);
    ASSERT_EQ(8, nfa->stateSet.size());

    FANode *n0 = nfa->stateSet[0];
    ASSERT_TRUE(n0 != nullptr);
    FANode *n1 = nfa->stateSet[1];
    ASSERT_TRUE(n1 != nullptr);
    FANode *n2 = nfa->stateSet[2];
    ASSERT_TRUE(n2 != nullptr);
    FANode *n3 = nfa->stateSet[3];
    ASSERT_TRUE(n3 != nullptr);
    FANode *n4 = nfa->stateSet[4];
    ASSERT_TRUE(n4 != nullptr);
    FANode *n5 = nfa->stateSet[5];
    ASSERT_TRUE(n5 != nullptr);
    FANode *n6 = nfa->stateSet[6];
    ASSERT_TRUE(n6 != nullptr);
    FANode *n7 = nfa->stateSet[7];
    ASSERT_TRUE(n7 != nullptr);

    ASSERT_EQ(6, nfa->initalState);
    ASSERT_EQ(n6, nfa->start);
    ASSERT_EQ(n7, nfa->accept);

    ASSERT_EQ(1, n0->edges.size());
    ASSERT_EQ('a', n0->edges[0].lable);
    ASSERT_EQ(n1, n0->edges[0].destination);

    ASSERT_EQ(1, n1->edges.size());
    ASSERT_EQ('\0', n1->edges[0].lable);
    ASSERT_EQ(n5, n1->edges[0].destination);

    ASSERT_EQ(1, n2->edges.size());
    ASSERT_EQ('b', n2->edges[0].lable);
    ASSERT_EQ(n3, n2->edges[0].destination);

    ASSERT_EQ(1, n3->edges.size());
    ASSERT_EQ('\0', n3->edges[0].lable);
    ASSERT_EQ(n5, n3->edges[0].destination);
    
    ASSERT_EQ(2, n4->edges.size());
    ASSERT_EQ('\0', n4->edges[0].lable);
    ASSERT_EQ(n0, n4->edges[0].destination);
    ASSERT_EQ('\0', n4->edges[1].lable);
    ASSERT_EQ(n2, n4->edges[1].destination);

    ASSERT_EQ(2, n5->edges.size());
    ASSERT_EQ('\0', n5->edges[0].lable);
    ASSERT_EQ(n4, n5->edges[0].destination);
    ASSERT_EQ('\0', n5->edges[1].lable);
    ASSERT_EQ(n7, n5->edges[1].destination);

    ASSERT_EQ(2, n6->edges.size());
    ASSERT_EQ('\0', n6->edges[0].lable);
    ASSERT_EQ(n4, n6->edges[0].destination);
    ASSERT_EQ('\0', n6->edges[1].lable);
    ASSERT_EQ(n7, n6->edges[1].destination);

    ASSERT_EQ(0, n7->edges.size());

    std::set<char> inputAlphabet = nfa->alphabet;
    std::set<char> testSet;
    testSet.insert('a');
    testSet.insert('b');
    ASSERT_EQ(testSet, inputAlphabet);
}

TEST(NFABuilderTest, aZeroOrOne)
{
    //a?
    AST *ast = new AST;
    Node *node = new Node;
    node->leftChild = node->rightChild = nullptr;
    node->token = {TokenType::ZERO_OR_ONE, "?"};
    ast->topNode = node;

    Node *a = new Node;
    a->leftChild = a->rightChild = nullptr;
    a->token = {TokenType::CHAR, "a"};
    node->rightChild = a;

    NFABuilder builder;
    NFA *nfa = builder.fromAST(ast);
    ASSERT_TRUE(nfa != nullptr);
    ASSERT_EQ(6, nfa->stateSet.size());

    FANode *n0 = nfa->stateSet[0];
    ASSERT_TRUE(n0 != nullptr);
    FANode *n1 = nfa->stateSet[1];
    ASSERT_TRUE(n1 != nullptr);
    FANode *n2 = nfa->stateSet[2];
    ASSERT_TRUE(n2 != nullptr);
    FANode *n3 = nfa->stateSet[3];
    ASSERT_TRUE(n3 != nullptr);
    FANode *n4 = nfa->stateSet[4];
    ASSERT_TRUE(n4 != nullptr);
    FANode *n5 = nfa->stateSet[5];
    ASSERT_TRUE(n5 != nullptr);

    ASSERT_EQ(n1->edges.size(), 1);
    ASSERT_EQ(n1->edges[0].destination, n5);
    ASSERT_EQ(n1->edges[0].lable, '\0');
    ASSERT_EQ(n2->state, nfa->initalState);
    ASSERT_EQ(n2->edges.size(), 2);
    ASSERT_EQ(n2->edges[0].destination, n0);
    ASSERT_EQ(n2->edges[0].lable, '\0');
    ASSERT_EQ(n2->edges[1].destination, n3);
    ASSERT_EQ(n2->edges[1].lable, '\0');
    ASSERT_EQ(n3->edges.size(), 1);
    ASSERT_EQ(n3->edges[0].destination, n4);
    ASSERT_EQ(n3->edges[0].lable, '\0');
    ASSERT_EQ(n4->edges.size(), 1);
    ASSERT_EQ(n4->edges[0].destination, n5);
    ASSERT_EQ(n4->edges[0].lable, '\0');
    ASSERT_EQ(n5->edges.size(), 0);
    ASSERT_TRUE(n5->isAcceptState);
}

TEST(NFABuilderTest, mix)
{
    //(a|b)*baa 
    AST *ast = new AST;
    Node *node = new Node;
    node->leftChild = node->rightChild = nullptr;
    node->token = {TokenType::CAT, ""};
    ast->topNode = node;

    Node *lastA = new Node;
    lastA->leftChild = lastA->rightChild = nullptr;
    lastA->token = {TokenType::CHAR, "a"};
    node->rightChild = lastA;

    Node *cat1 = new Node;
    cat1->leftChild = cat1->rightChild = nullptr;
    cat1->token = {TokenType::CAT, ""};
    node->leftChild = cat1;

    Node *secondA = new Node;
    secondA->leftChild = secondA->rightChild = nullptr;
    secondA->token = {TokenType::CHAR, "a"};
    cat1->rightChild = secondA;

    Node *cat2 = new Node;
    cat2->leftChild = cat2->rightChild = nullptr;
    cat2->token = {TokenType::CAT, ""};
    cat1->leftChild = cat2;

    Node *secondB = new Node;
    secondB->leftChild = secondB->rightChild = nullptr;
    secondB->token = {TokenType::CHAR, "b"};
    cat2->rightChild = secondB;

    Node *star = new Node;
    star->leftChild = star->rightChild = nullptr;
    star->token = {TokenType::ZERO_MORE, "*"};
    cat2->leftChild = star;

    Node *or_ = new Node;
    or_->leftChild = or_->rightChild = nullptr;
    or_->token = {TokenType::OR, "|"};
    star->leftChild = or_;

    Node *a = new Node;
    a->leftChild = a->rightChild = nullptr;
    a->token = {TokenType::CHAR, "a"};
    or_->leftChild = a;

    Node *b = new Node;
    b->leftChild = b->rightChild = nullptr;
    b->token = {TokenType::OR, "b"};
    or_->rightChild = b;

    NFABuilder builder;
    NFA *nfa = builder.fromAST(ast);
    ASSERT_TRUE(nfa != nullptr);
    ASSERT_EQ(14, nfa->stateSet.size());

    ASSERT_EQ(6, nfa->start->state);
    ASSERT_EQ(6, nfa->initalState);
    ASSERT_EQ(2, nfa->start->edges.size());
    FANode *n4 = nfa->start->edges[0].destination;
    FANode *n7 = nfa->start->edges[1].destination;
    ASSERT_EQ(4, n4->state);
    FANode *n5 = n4->edges[0].destination->edges[0].destination->edges[0].destination;
    ASSERT_EQ(5, n5->state);
    ASSERT_EQ(2, n5->edges.size());
    ASSERT_EQ(n4, n5->edges[0].destination);
    ASSERT_EQ(n7, n5->edges[1].destination);
    ASSERT_EQ(13, nfa->accept->state);

    std::set<char> inputAlphabet = nfa->alphabet;
    std::set<char> testSet;
    testSet.insert('a');
    testSet.insert('b');
    ASSERT_EQ(testSet, inputAlphabet);
}
