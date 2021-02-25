#include <CommonDataStructure/FA.hpp>
#include <Compiler/Compiler.hpp>
#include <gtest/gtest.h>

TEST(CompilerTest, singleCharacter)
{
    Compiler compiler;
    DFA *dfa = compiler.compile("a");
    ASSERT_NE(nullptr, dfa);
    ASSERT_EQ(dfa->stateSet.size(), 2);
    FANode *na = dfa->stateSet['A'];
    ASSERT_NE(nullptr, na);
    ASSERT_EQ(dfa->start, na);
    FANode *nb = dfa->stateSet['B'];
    ASSERT_NE(nullptr, nb);
    ASSERT_EQ(dfa->accept, nb);

    ASSERT_EQ(1, na->edges.size());
    ASSERT_EQ('a', na->edges[0].lable.lowerBound);
    ASSERT_EQ('a', na->edges[0].lable.higherBound);
    ASSERT_EQ(nb, na->edges[0].destination);
    ASSERT_EQ(0, nb->edges.size());
}

TEST(CompilerTest, closure)
{
    Compiler compiler;
    DFA *dfa = compiler.compile("a*");
    ASSERT_NE(nullptr, dfa);
    ASSERT_EQ(dfa->stateSet.size(), 2);
    FANode *na = dfa->stateSet['A'];
    ASSERT_NE(nullptr, na);
    ASSERT_EQ(dfa->start, na);
    FANode *nb = dfa->stateSet['B'];
    ASSERT_NE(nullptr, nb);
    ASSERT_EQ(dfa->accept, nb);

    ASSERT_EQ(1, na->edges.size());
    ASSERT_EQ('a', na->edges[0].lable.lowerBound);
    ASSERT_EQ(nb, na->edges[0].destination);
    ASSERT_EQ(1, nb->edges.size());
    ASSERT_EQ('a', nb->edges[0].lable.lowerBound);
}

TEST(CompilerTest, concatenation)
{
    Compiler compiler;
    DFA *dfa = compiler.compile("ab");
    ASSERT_NE(nullptr, dfa);
    ASSERT_EQ(dfa->stateSet.size(), 3);
    FANode *na = dfa->stateSet['A'];
    ASSERT_NE(nullptr, na);
    FANode *nb = dfa->stateSet['B'];
    ASSERT_NE(nullptr, nb);
    ASSERT_FALSE(nb->isAcceptState);
    FANode *nc = dfa->stateSet['C'];
    ASSERT_NE(nullptr, nc);
    ASSERT_EQ(dfa->accept, nc);
    ASSERT_TRUE(nc->isAcceptState);

    ASSERT_EQ(1, na->edges.size());
    ASSERT_EQ('a', na->edges[0].lable.lowerBound);
    ASSERT_EQ(nb, na->edges[0].destination);
    ASSERT_EQ(1, nb->edges.size());
    ASSERT_EQ('b', nb->edges[0].lable.lowerBound);
    ASSERT_EQ(nc, nb->edges[0].destination);
    ASSERT_EQ(0, nc->edges.size());
}

TEST(CompilerTest, threeAcceptState)
{
    Compiler compiler;
    DFA *dfa = compiler.compile("(a|b)*");
    ASSERT_NE(nullptr, dfa);
    ASSERT_EQ(dfa->stateSet.size(), 3);
    FANode *na = dfa->stateSet['A'];
    ASSERT_NE(nullptr, na);
    ASSERT_TRUE(na->isAcceptState);

    FANode *nb = dfa->stateSet['B'];
    ASSERT_NE(nullptr, nb);
    ASSERT_TRUE(nb->isAcceptState);

    FANode *nc = dfa->stateSet['C'];
    ASSERT_NE(nullptr, nc);
    ASSERT_TRUE(nc->isAcceptState);
}

TEST(CompilerTest, threeAcceptState2)
{
    Compiler compiler;
    DFA *dfa = compiler.compile("(a*|b*)*");
    ASSERT_NE(nullptr, dfa);
    ASSERT_EQ(dfa->stateSet.size(), 3);
    FANode *na = dfa->stateSet['A'];
    ASSERT_NE(nullptr, na);
    ASSERT_TRUE(na->isAcceptState);

    FANode *nb = dfa->stateSet['B'];
    ASSERT_NE(nullptr, nb);
    ASSERT_TRUE(nb->isAcceptState);

    FANode *nc = dfa->stateSet['C'];
    ASSERT_NE(nullptr, nc);
    ASSERT_TRUE(nc->isAcceptState);
}

TEST(CompilerTest, fiveAcceptState)
{
    Compiler compiler;
    DFA *dfa = compiler.compile("(a|b)*abb(a|b)*");
    ASSERT_NE(nullptr, dfa);
    ASSERT_EQ(dfa->stateSet.size(), 9);
    ASSERT_TRUE(dfa->stateSet['E']->isAcceptState);
    ASSERT_TRUE(dfa->stateSet['F']->isAcceptState);
    ASSERT_TRUE(dfa->stateSet['H']->isAcceptState);
    ASSERT_TRUE(dfa->stateSet['I']->isAcceptState);
    ASSERT_TRUE(dfa->stateSet['G']->isAcceptState);
}