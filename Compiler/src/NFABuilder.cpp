#include <CommonDataStructure/AST.hpp>
#include <CommonDataStructure/FA.hpp>
#include <CommonDataStructure/Token.hpp>
#include <Compiler/NFABuilder.hpp>
#include <vector>

bool isLeaf(Node *node)
{
    return node->leftChild == nullptr && node->rightChild == nullptr;
}

struct NFABuilder::Impl
{
    std::vector<NFA *> stack;

    unsigned char nextState = 0;

    FANode *CreateNFANode(NFA *nfa)
    {
        FANode *node = new FANode;
        if (node == nullptr)
        {
            abort();
        }
        node->state = nextState++;
        nfa->stateSet[node->state] = node;
        return node;
    }

    void leaf(Token token)
    {
        NFA *nfa = new NFA;
        nfa->start = CreateNFANode(nfa);
        nfa->accept = CreateNFANode(nfa);
        nfa->start->edges.emplace_back(nfa->accept, token.value[0]);
        FASymbol symbol = {token.value[0], token.value[0]};
        nfa->alphabet.insert(symbol);
        stack.push_back(nfa);
    }

    void star()
    {
        NFA *nfa = stack[stack.size() - 1];
        stack.pop_back();

        FANode *start = CreateNFANode(nfa);
        FANode *accept = CreateNFANode(nfa);

        Edge toOldStart(nfa->start, '\0');
        Edge toNewAccept(accept, '\0');
        start->edges.push_back(toOldStart);
        start->edges.push_back(toNewAccept);
        nfa->accept->edges.push_back(toOldStart);
        nfa->accept->edges.push_back(toNewAccept);

        nfa->start = start;
        nfa->accept = accept;
        stack.push_back(nfa);
    }

    NFA *buildEmptyNFA()
    {
        NFA *nfa = new NFA;
        nfa->start = CreateNFANode(nfa);
        nfa->accept = CreateNFANode(nfa);

        Edge edge(nfa->accept, '\0');
        nfa->start->edges.push_back(edge);
        return nfa;
    }

    void BuildNFA(Node *astNode)
    {
        if (isLeaf(astNode))
        {
            leaf(astNode->token);
            return;
        }
        if (astNode->leftChild)
        {
            BuildNFA(astNode->leftChild);
        }
        if (astNode->rightChild)
        {
            BuildNFA(astNode->rightChild);
        }
        if (astNode->token.type == TokenType::OR)
        {
            NFA *nfa = new NFA;
            nfa->start = CreateNFANode(nfa);
            nfa->accept = CreateNFANode(nfa);

            NFA *nfa2 = stack[stack.size() - 1];
            stack.pop_back();
            NFA *nfa1 = stack[stack.size() - 1];
            stack.pop_back();

            Edge edge1(nfa1->start, '\0');
            nfa->start->edges.push_back(edge1);
            Edge edge2(nfa2->start, '\0');
            nfa->start->edges.push_back(edge2);

            Edge edge3(nfa->accept, '\0');
            nfa1->accept->edges.push_back(edge3);
            Edge edge4(nfa->accept, '\0');
            nfa2->accept->edges.push_back(edge4);


            nfa->alphabet.insert(nfa1->alphabet.begin(), nfa1->alphabet.end());
            nfa->alphabet.insert(nfa2->alphabet.begin(), nfa2->alphabet.end());
            nfa->stateSet.insert(nfa1->stateSet.begin(), nfa1->stateSet.end());
            nfa->stateSet.insert(nfa2->stateSet.begin(), nfa2->stateSet.end());

            delete nfa1;
            delete nfa2;
            stack.push_back(nfa);
            return;
        }

        if (astNode->token.type == TokenType::CAT)
        {
            NFA *nfa2 = stack[stack.size() - 1];
            stack.pop_back();
            NFA *nfa1 = stack[stack.size() - 1];
            stack.pop_back();

            Edge edge(nfa2->start, '\0');
            nfa1->accept->edges.push_back(edge);
            nfa1->accept = nfa2->accept;
            nfa1->alphabet.insert(nfa2->alphabet.begin(), nfa2->alphabet.end());
            nfa1->stateSet.insert(nfa2->stateSet.begin(), nfa2->stateSet.end());
            delete nfa2;
            stack.push_back(nfa1);
            return;
        }

        if (astNode->token.type == TokenType::ZERO_MORE)
        {
            star();
            return;
        }

        if (astNode->token.type == TokenType::ZERO_OR_ONE)
        {
            NFA *nfa = stack[stack.size() - 1];
            stack.pop_back();
            //create four node
            FANode *n0 = CreateNFANode(nfa);
            FANode *n1 = CreateNFANode(nfa);
            FANode *n2 = CreateNFANode(nfa);
            FANode *n3 = CreateNFANode(nfa);

            n3->isAcceptState = true;
            Edge toOladStart(nfa->start, '\0');
            Edge ton1(n1, '\0');
            Edge ton2(n2, '\0');
            Edge toNewAccept(n3, '\0');
            n0->edges.push_back(toOladStart);
            n0->edges.push_back(ton1);
            n1->edges.push_back(ton2);
            n2->edges.push_back(toNewAccept);
            nfa->accept->edges.push_back(toNewAccept);

            //reset new start and accept
            nfa->start = n0;
            nfa->accept = n3;

            stack.push_back(nfa);
            return;
        }
    }
};

NFABuilder::NFABuilder()
    : impl_(new Impl)
{
}

NFABuilder::~NFABuilder() = default;

NFA *NFABuilder::fromAST(AST *ast)
{
    if (ast == nullptr){
        return impl_->buildEmptyNFA();
    }
    impl_->BuildNFA(ast->topNode);
    if (impl_->stack.size() != 1)
    {
        return nullptr;
    }
    return impl_->stack[0];
}