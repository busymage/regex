#include <CommonDataStructure/FA.hpp>
#include <Compiler/DFABuilder.hpp>
#include <gtest/gtest.h>
#include <set>
#include <vector>
#include <printf.h>

TEST(DFABuilderTest, epsilonClosure)
{
    // (a|b)*baa
    NFA *nfa = new NFA;
    std::vector<FANode*> nfaNodes;
    for (size_t i = 0; i < 14; i++)
    {
        FANode *n = new FANode;
        n->state = i;
        nfaNodes.push_back(n);
    }
    FANode *n0 = nfaNodes[0];
    FANode *n1 = nfaNodes[1];
    FANode *n2 = nfaNodes[2];
    FANode *n3 = nfaNodes[3];
    FANode *n4 = nfaNodes[4];
    FANode *n5 = nfaNodes[5];
    FANode *n6 = nfaNodes[6];
    FANode *n7 = nfaNodes[7];
    FANode *n8 = nfaNodes[8];
    FANode *n9 = nfaNodes[9];
    FANode *n10 = nfaNodes[10];
    FANode *n11 = nfaNodes[11];
    FANode *n12 = nfaNodes[12];
    FANode *n13 = nfaNodes[13];

    Edge e = {n1, 'a'};
    n0->edges.push_back(e);
    e = {n5, '\0'};
    n1->edges.push_back(e);
    n3->edges.push_back(e);
    e = {n3, 'b'};
    n2->edges.push_back(e);
    e = {n0, '\0'};
    n4->edges.push_back(e);
    e = {n2, '\0'};
    n4->edges.push_back(e);
    e = {n4, '\0'};
    n5->edges.push_back(e);
    n6->edges.push_back(e);
    e = {n7, '\0'};
    n5->edges.push_back(e);
    n6->edges.push_back(e);
    e = {n8, '\0'};
    n7->edges.push_back(e);
    e = {n9, 'b'};
    n8->edges.push_back(e);
    e = {n10, '\0'};
    n9->edges.push_back(e);
    e = {n11, 'a'};
    n10->edges.push_back(e);
    e = {n12, '\0'};
    n11->edges.push_back(e);
    e = {n13, 'a'};
    n12->edges.push_back(e);

    nfa->start = n6;
    nfa->accept = n13;

    DFABuilder builder(nfa);
    std::set<unsigned char> states = builder.epsilonClosure(nfa->start);
    ASSERT_EQ(states.size(), 6);
    std::set<unsigned char> testSet = {0,2,4,6,7,8};
    ASSERT_EQ(states, testSet);
}

TEST(DFABuilderTest, emptyInputForEpsilonClosure)
{
    DFABuilder builder(nullptr);
    std::set<unsigned char> states = builder.epsilonClosure(nullptr);
    ASSERT_TRUE(states.empty());
}

TEST(DFABuilderTest, move)
{
    // (a|b)*baa
    NFA *nfa = new NFA;
    std::vector<FANode*> nfaNodes;
    for (size_t i = 0; i < 14; i++)
    {
        FANode *n = new FANode;
        n->state = i;
        nfaNodes.push_back(n);
    }
    FANode *n0 = nfaNodes[0];
    FANode *n1 = nfaNodes[1];
    FANode *n2 = nfaNodes[2];
    FANode *n3 = nfaNodes[3];
    FANode *n4 = nfaNodes[4];
    FANode *n5 = nfaNodes[5];
    FANode *n6 = nfaNodes[6];
    FANode *n7 = nfaNodes[7];
    FANode *n8 = nfaNodes[8];
    FANode *n9 = nfaNodes[9];
    FANode *n10 = nfaNodes[10];
    FANode *n11 = nfaNodes[11];
    FANode *n12 = nfaNodes[12];
    FANode *n13 = nfaNodes[13];

    Edge e = {n1, 'a'};
    n0->edges.push_back(e);
    e = {n5, '\0'};
    n1->edges.push_back(e);
    n3->edges.push_back(e);
    e = {n3, 'b'};
    n2->edges.push_back(e);
    e = {n0, '\0'};
    n4->edges.push_back(e);
    e = {n2, '\0'};
    n4->edges.push_back(e);
    e = {n4, '\0'};
    n5->edges.push_back(e);
    n6->edges.push_back(e);
    e = {n7, '\0'};
    n5->edges.push_back(e);
    n6->edges.push_back(e);
    e = {n8, '\0'};
    n7->edges.push_back(e);
    e = {n9, 'b'};
    n8->edges.push_back(e);
    e = {n10, '\0'};
    n9->edges.push_back(e);
    e = {n11, 'a'};
    n10->edges.push_back(e);
    e = {n12, '\0'};
    n11->edges.push_back(e);
    e = {n13, 'a'};
    n12->edges.push_back(e);

    nfa->start = n6;
    nfa->accept = n13;
    FASymbol symbola = {'a','a'};
    FASymbol symbolb = {'b','b'};
    nfa->alphabet.insert(symbola);
    nfa->alphabet.insert(symbolb);
    for (size_t i = 0; i < nfaNodes.size(); i++)
    {
        nfa->stateSet[i] = nfaNodes[i];
    }

    DFABuilder builder(nfa);
    std::set<unsigned char> states = builder.epsilonClosure(nfa->start);
    
    std::set<unsigned char> states1 = builder.move(states, symbola);
    std::set<unsigned char> testSet = {1};
    ASSERT_EQ(states1, testSet);
    
    states1 = builder.move(states, symbolb);
    testSet = {3,9};
    ASSERT_EQ(states1, testSet);
}

TEST(DFABuilderTest, epsilonClosureWithSet)
{
    // (a|b)*baa
    NFA *nfa = new NFA;
    std::vector<FANode*> nfaNodes;
    for (size_t i = 0; i < 14; i++)
    {
        FANode *n = new FANode;
        n->state = i;
        nfaNodes.push_back(n);
    }
    FANode *n0 = nfaNodes[0];
    FANode *n1 = nfaNodes[1];
    FANode *n2 = nfaNodes[2];
    FANode *n3 = nfaNodes[3];
    FANode *n4 = nfaNodes[4];
    FANode *n5 = nfaNodes[5];
    FANode *n6 = nfaNodes[6];
    FANode *n7 = nfaNodes[7];
    FANode *n8 = nfaNodes[8];
    FANode *n9 = nfaNodes[9];
    FANode *n10 = nfaNodes[10];
    FANode *n11 = nfaNodes[11];
    FANode *n12 = nfaNodes[12];
    FANode *n13 = nfaNodes[13];

    Edge e = {n1, 'a'};
    n0->edges.push_back(e);
    e = {n5, '\0'};
    n1->edges.push_back(e);
    n3->edges.push_back(e);
    e = {n3, 'b'};
    n2->edges.push_back(e);
    e = {n0, '\0'};
    n4->edges.push_back(e);
    e = {n2, '\0'};
    n4->edges.push_back(e);
    e = {n4, '\0'};
    n5->edges.push_back(e);
    n6->edges.push_back(e);
    e = {n7, '\0'};
    n5->edges.push_back(e);
    n6->edges.push_back(e);
    e = {n8, '\0'};
    n7->edges.push_back(e);
    e = {n9, 'b'};
    n8->edges.push_back(e);
    e = {n10, '\0'};
    n9->edges.push_back(e);
    e = {n11, 'a'};
    n10->edges.push_back(e);
    e = {n12, '\0'};
    n11->edges.push_back(e);
    e = {n13, 'a'};
    n12->edges.push_back(e);

    nfa->start = n6;
    nfa->accept = n13;
    FASymbol symbola = {'a','a'};
    FASymbol symbolb = {'b','b'};
    nfa->alphabet.insert(symbola);
    nfa->alphabet.insert(symbolb);
    for (size_t i = 0; i < nfaNodes.size(); i++)
    {
        nfa->stateSet[i] = nfaNodes[i];
    }

    DFABuilder builder(nfa);
    std::set<unsigned char> states = {3, 9};
    std::set<unsigned char> testStates = {0, 2, 3, 4, 5, 7, 8, 9, 10};
    std::set<unsigned char> retStates = builder.epsilonClosure(states);
    ASSERT_EQ(retStates, testStates);

    states = {1, 11};
    testStates = {0, 1, 2, 4, 5, 7, 8, 11, 12};
    retStates = builder.epsilonClosure(states);
    ASSERT_EQ(retStates, testStates);
}

TEST(DFABuilderTest, build)
{
      // (a|b)*baa
    NFA *nfa = new NFA;
    std::vector<FANode*> nfaNodes;
    for (size_t i = 0; i < 14; i++)
    {
        FANode *n = new FANode;
        n->state = i;
        nfaNodes.push_back(n);
    }
    FANode *n0 = nfaNodes[0];
    FANode *n1 = nfaNodes[1];
    FANode *n2 = nfaNodes[2];
    FANode *n3 = nfaNodes[3];
    FANode *n4 = nfaNodes[4];
    FANode *n5 = nfaNodes[5];
    FANode *n6 = nfaNodes[6];
    FANode *n7 = nfaNodes[7];
    FANode *n8 = nfaNodes[8];
    FANode *n9 = nfaNodes[9];
    FANode *n10 = nfaNodes[10];
    FANode *n11 = nfaNodes[11];
    FANode *n12 = nfaNodes[12];
    FANode *n13 = nfaNodes[13];

    Edge e = {n1, 'a'};
    n0->edges.push_back(e);
    e = {n5, '\0'};
    n1->edges.push_back(e);
    n3->edges.push_back(e);
    e = {n3, 'b'};
    n2->edges.push_back(e);
    e = {n0, '\0'};
    n4->edges.push_back(e);
    e = {n2, '\0'};
    n4->edges.push_back(e);
    e = {n4, '\0'};
    n5->edges.push_back(e);
    n6->edges.push_back(e);
    e = {n7, '\0'};
    n5->edges.push_back(e);
    n6->edges.push_back(e);
    e = {n8, '\0'};
    n7->edges.push_back(e);
    e = {n9, 'b'};
    n8->edges.push_back(e);
    e = {n10, '\0'};
    n9->edges.push_back(e);
    e = {n11, 'a'};
    n10->edges.push_back(e);
    e = {n12, '\0'};
    n11->edges.push_back(e);
    e = {n13, 'a'};
    n12->edges.push_back(e);

    nfa->start = n6;
    nfa->accept = n13;
    FASymbol symbola = {'a','a'};
    FASymbol symbolb = {'b','b'};
    nfa->alphabet.insert(symbola);
    nfa->alphabet.insert(symbolb);
    for (size_t i = 0; i < nfaNodes.size(); i++)
    {
        nfa->stateSet[i] = nfaNodes[i];
    }

    DFABuilder builder(nfa);
    DFA *dfa = builder.build();
    ASSERT_EQ('A', dfa->start->state);
    ASSERT_EQ('E', dfa->accept->state);
    ASSERT_EQ(5, dfa->stateSet.size());

    FANode *na = dfa->stateSet['A'];
    ASSERT_NE(nullptr, na);
    ASSERT_EQ(2, na->edges.size());
    FANode *nb = dfa->stateSet['B'];
    ASSERT_NE(nullptr, na);
    ASSERT_EQ(2, nb->edges.size());
    FANode *nc = dfa->stateSet['C'];
    ASSERT_NE(nullptr, na);
    ASSERT_EQ(2, nc->edges.size());
    FANode *nd = dfa->stateSet['D'];
    ASSERT_NE(nullptr, na);
    ASSERT_EQ(2, nd->edges.size());
    FANode *ne = dfa->stateSet['E'];
    ASSERT_NE(nullptr, na);
    ASSERT_EQ(2, ne->edges.size());

    ASSERT_EQ('a', na->edges[0].lable.lowerBound);
    ASSERT_EQ('b', na->edges[1].lable.lowerBound);
    ASSERT_EQ(nb, na->edges[0].destination);
    ASSERT_EQ(nc, na->edges[1].destination);
    
    ASSERT_EQ('a', nb->edges[0].lable.lowerBound);
    ASSERT_EQ('b', nb->edges[1].lable.lowerBound);
    ASSERT_EQ(nb, na->edges[0].destination);
    ASSERT_EQ(nc, na->edges[1].destination);

    ASSERT_EQ('a', nc->edges[0].lable.lowerBound);
    ASSERT_EQ('b', nc->edges[1].lable.lowerBound);
    ASSERT_EQ(nd, nc->edges[0].destination);
    ASSERT_EQ(nc, nc->edges[1].destination);

    ASSERT_EQ('a', nd->edges[0].lable.lowerBound);
    ASSERT_EQ('b', nd->edges[1].lable.lowerBound);
    ASSERT_EQ(ne, nd->edges[0].destination);
    ASSERT_EQ(nc, nd->edges[1].destination);

    ASSERT_EQ('a', ne->edges[0].lable.lowerBound);
    ASSERT_EQ('b', ne->edges[1].lable.lowerBound);
    ASSERT_EQ(nb, ne->edges[0].destination);
    ASSERT_EQ(nc, ne->edges[1].destination);
}

TEST(DFABuilderTest, buildMultipleTimeGetSameDfa)
{
      // (a|b)*baa
    NFA *nfa = new NFA;
    std::vector<FANode*> nfaNodes;
    for (size_t i = 0; i < 14; i++)
    {
        FANode *n = new FANode;
        n->state = i;
        nfaNodes.push_back(n);
    }
    FANode *n0 = nfaNodes[0];
    FANode *n1 = nfaNodes[1];
    FANode *n2 = nfaNodes[2];
    FANode *n3 = nfaNodes[3];
    FANode *n4 = nfaNodes[4];
    FANode *n5 = nfaNodes[5];
    FANode *n6 = nfaNodes[6];
    FANode *n7 = nfaNodes[7];
    FANode *n8 = nfaNodes[8];
    FANode *n9 = nfaNodes[9];
    FANode *n10 = nfaNodes[10];
    FANode *n11 = nfaNodes[11];
    FANode *n12 = nfaNodes[12];
    FANode *n13 = nfaNodes[13];

    Edge e = {n1, 'a'};
    n0->edges.push_back(e);
    e = {n5, '\0'};
    n1->edges.push_back(e);
    n3->edges.push_back(e);
    e = {n3, 'b'};
    n2->edges.push_back(e);
    e = {n0, '\0'};
    n4->edges.push_back(e);
    e = {n2, '\0'};
    n4->edges.push_back(e);
    e = {n4, '\0'};
    n5->edges.push_back(e);
    n6->edges.push_back(e);
    e = {n7, '\0'};
    n5->edges.push_back(e);
    n6->edges.push_back(e);
    e = {n8, '\0'};
    n7->edges.push_back(e);
    e = {n9, 'b'};
    n8->edges.push_back(e);
    e = {n10, '\0'};
    n9->edges.push_back(e);
    e = {n11, 'a'};
    n10->edges.push_back(e);
    e = {n12, '\0'};
    n11->edges.push_back(e);
    e = {n13, 'a'};
    n12->edges.push_back(e);

    nfa->start = n6;
    nfa->accept = n13;
    FASymbol symbola = {'a','a'};
    FASymbol symbolb = {'b','b'};
    nfa->alphabet.insert(symbola);
    nfa->alphabet.insert(symbolb);
    for (size_t i = 0; i < nfaNodes.size(); i++)
    {
        nfa->stateSet[i] = nfaNodes[i];
    }

    DFABuilder builder(nfa);
    DFA *dfa1 = builder.build();
    DFA *dfa2 = builder.build();
    DFA *dfa3 = builder.build();
    ASSERT_EQ(5, dfa1->stateSet.size());
    ASSERT_EQ(5, dfa2->stateSet.size());
    ASSERT_EQ(5, dfa3->stateSet.size());
}

TEST(DFABuilderTest, buildaStar)
{
    NFA *nfa = new NFA;
    std::vector<FANode*> nfaNodes;
    for (size_t i = 0; i < 4; i++)
    {
        FANode *n = new FANode;
        n->state = i;
        nfaNodes.push_back(n);
    }
    FANode *n0 = nfaNodes[0];
    FANode *n1 = nfaNodes[1];
    FANode *n2 = nfaNodes[2];
    FANode *n3 = nfaNodes[3];
    FANode *n5 = nfaNodes[5];
    FANode *n6 = nfaNodes[6];
    FANode *n7 = nfaNodes[7];
    n0->edges.push_back((Edge){n1, '\0'});
    n0->edges.push_back((Edge){n3, '\0'});
    n1->edges.push_back((Edge){n2, 'a'});
    n2->edges.push_back((Edge){n1, '\0'});
    n2->edges.push_back((Edge){n3, '\0'});

    nfa->start = n0;
    nfa->accept = n3;
    FASymbol symbola = {'a','a'};
    nfa->alphabet.insert(symbola);
    for (size_t i = 0; i < nfaNodes.size(); i++)
    {
        nfa->stateSet[i] = nfaNodes[i];
    }

    DFABuilder builder(nfa);
    DFA *dfa1 = builder.build();
    ASSERT_NE(dfa1, nullptr);
    ASSERT_EQ(dfa1->stateSet.size(), 2);
    FANode *na = dfa1->stateSet['A'];
    FANode *nb = dfa1->stateSet['B'];
    ASSERT_NE(nullptr, na);
    ASSERT_NE(nullptr, nb);
    ASSERT_TRUE(na->isAcceptState);
    ASSERT_TRUE(nb->isAcceptState);
    ASSERT_EQ(na->edges.size(), 1);
    ASSERT_EQ(na->edges[0].destination, nb);
    ASSERT_EQ(na->edges[0].lable.lowerBound, 'a');
    ASSERT_EQ(nb->edges.size(), 1);
    ASSERT_EQ(nb->edges[0].destination, nb);
    ASSERT_EQ(nb->edges[0].lable.lowerBound, 'a');
}

TEST(DFABuilderTest, buildaOrbStar)
{
    //(a|b)*
    NFA *nfa = new NFA;
    std::vector<FANode*> nfaNodes;
    for (size_t i = 0; i < 8; i++)
    {
        FANode *n = new FANode;
        n->state = i;
        nfaNodes.push_back(n);
    }
    FANode *n0 = nfaNodes[0];
    FANode *n1 = nfaNodes[1];
    FANode *n2 = nfaNodes[2];
    FANode *n3 = nfaNodes[3];
    FANode *n4 = nfaNodes[4];
    FANode *n5 = nfaNodes[5];
    FANode *n6 = nfaNodes[6];
    FANode *n7 = nfaNodes[7];
    n0->edges.push_back((Edge){n1, '\0'});
    n0->edges.push_back((Edge){n7, '\0'});
    n1->edges.push_back((Edge){n2, '\0'});
    n1->edges.push_back((Edge){n4, '\0'});
    n2->edges.push_back((Edge){n3, 'a'});
    n3->edges.push_back((Edge){n6, '\0'});
    n4->edges.push_back((Edge){n5, 'b'});
    n5->edges.push_back((Edge){n6, '\0'});
    n6->edges.push_back((Edge){n1, '\0'});
    n6->edges.push_back((Edge){n7, '\0'});

    nfa->start = n0;
    nfa->accept = n7;
    FASymbol symbola = {'a','a'};
    FASymbol symbolb = {'b','b'};
    nfa->alphabet.insert(symbola);
    nfa->alphabet.insert(symbolb);
    for (size_t i = 0; i < nfaNodes.size(); i++)
    {
        nfa->stateSet[i] = nfaNodes[i];
    }

    DFABuilder builder(nfa);
    DFA *dfa1 = builder.build();
    ASSERT_NE(dfa1, nullptr);
    ASSERT_EQ(dfa1->stateSet.size(), 3);
    FANode *na = dfa1->stateSet['A'];
    FANode *nb = dfa1->stateSet['B'];
    FANode *nc = dfa1->stateSet['C'];
    ASSERT_NE(nullptr, na);
    ASSERT_NE(nullptr, nb);
    ASSERT_NE(nullptr, nc);
    ASSERT_TRUE(na->isAcceptState);
    ASSERT_TRUE(nb->isAcceptState);
    ASSERT_TRUE(nc->isAcceptState);
    ASSERT_EQ(na->edges.size(), 2);
    ASSERT_EQ(na->edges[0].destination, nb);
    ASSERT_EQ(na->edges[0].lable.lowerBound, 'a');
    ASSERT_EQ(na->edges[1].destination, nc);
    ASSERT_EQ(na->edges[1].lable.lowerBound, 'b');
    ASSERT_EQ(nb->edges.size(), 2);
    ASSERT_EQ(nb->edges[0].destination, nb);
    ASSERT_EQ(nb->edges[0].lable.lowerBound, 'a');
    ASSERT_EQ(nb->edges[1].destination, nc);
    ASSERT_EQ(nb->edges[1].lable.lowerBound, 'b');
    ASSERT_EQ(nc->edges.size(), 2);
    ASSERT_EQ(nc->edges[0].destination, nb);
    ASSERT_EQ(nc->edges[0].lable.lowerBound, 'a');
    ASSERT_EQ(nc->edges[1].destination, nc);
    ASSERT_EQ(nc->edges[1].lable.lowerBound, 'b');
}

TEST(DFABuilderTest, buildaQustionMark)
{
   //a?
    NFA *nfa = new NFA;
    std::vector<FANode*> nfaNodes;
    for (size_t i = 0; i < 6; i++)
    {
        FANode *n = new FANode;
        n->state = i;
        nfaNodes.push_back(n);
        nfa->stateSet[i] = n;
    }
    FANode *n0 = nfaNodes[0];
    FANode *n1 = nfaNodes[1];
    FANode *n2 = nfaNodes[2];
    FANode *n3 = nfaNodes[3];
    FANode *n4 = nfaNodes[4];
    FANode *n5 = nfaNodes[5];
    n3->isAcceptState = true;
    Edge ton1 = {n1, '\0'};
    Edge ton2 = {n2, 'a'};
    Edge ton3 = {n3, '\0'};
    Edge ton4 = {n4, '\0'};
    Edge ton5 = {n5, '\0'};
    n0->edges.push_back(ton1);
    n0->edges.push_back(ton3);
    n1->edges.push_back(ton2);
    n2->edges.push_back(ton5);
    n3->edges.push_back(ton4);
    n4->edges.push_back(ton5);

    nfa->start = n0;
    nfa->accept = n5;
    FASymbol symbola = {'a','a'};
    nfa->alphabet.insert(symbola);

    DFABuilder builder(nfa);
    DFA *dfa1 = builder.build();
    ASSERT_NE(dfa1, nullptr);
    ASSERT_EQ(dfa1->stateSet.size(), 2);
    FANode *na = dfa1->stateSet['A'];
    FANode *nb = dfa1->stateSet['B'];
    ASSERT_TRUE(na->isAcceptState);
    ASSERT_TRUE(nb->isAcceptState);
    ASSERT_EQ(na->edges.size(), 1);
    ASSERT_EQ(na->edges[0].destination, nb);
    ASSERT_EQ(na->edges[0].lable.lowerBound, 'a');
    ASSERT_EQ(nb->edges.size(), 0);
}