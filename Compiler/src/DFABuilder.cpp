#include <assert.h>
#include <Compiler/FA.hpp>
#include <Compiler/DFABuilder.hpp>
#include <map>
#include <memory>
#include <set>
#include <vector>

struct DFABuilder::Impl
{
    
    struct NodeInfo{
        FANode *fanode;
        FAState dfaState;
        std::set<FAState> nfaState;
        bool isMark = false;

        ~NodeInfo()
        {
            fanode = nullptr;
        }
    };

    struct Temp{
        FAState nextState = 'A';
        std::vector<std::shared_ptr<NodeInfo>> states;

        ~Temp()
        { 
        }
    };

    std::shared_ptr<NFA> nfa;
    std::shared_ptr<Temp> temp;

    FANode *CreateDFANode(std::shared_ptr<DFA>  dfa)
    {
        if(temp == nullptr){
            return nullptr;
        }
        FANode *node = new FANode;
        if (node == nullptr)
        {
            abort();
        }
        node->state = temp->nextState++;
        dfa->stateSet[node->state] = node;
        return node;
    }

    bool alreadyInStates(std::set<unsigned char> &nfaState)
    {
        if(temp == nullptr){
            return false;
        }
        for (auto info : temp->states)
        {
            if(info->nfaState == nfaState){
                return true;
            }
        }
        return false;
    }

    FANode *findNodeByStateSet(std::set<unsigned char> &nfaState)
    {
        for (auto info : temp->states)
        {
            if(info->nfaState == nfaState){
                return info->fanode;
            }
        }
        return nullptr;  
    }

    void addNewState(FANode *node, std::set<unsigned char> &nfaState)
    {
        if(temp == nullptr){
            return;
        }
        if(node == nullptr){
            return;
        }
        auto info = std::make_shared<NodeInfo>();
        info->fanode = node;
        info->dfaState = node->state;
        info->nfaState = nfaState;
        temp->states.push_back(info);
    }

    void epsilonClosure(FANode *node, std::set<unsigned char> &alreadyWalkNode)
    {
        alreadyWalkNode.insert(node->state);
        for (size_t i = 0; i < node->edges.size(); i++)
        {
            Edge e = node->edges[i];
            if (e.lable.lowerBound == '\0' && 
                e.lable.upperBound == '\0' &&
                alreadyWalkNode.count(e.destination->state) == 0)
            {
                epsilonClosure(e.destination, alreadyWalkNode);
            }
        }
    }
};

DFABuilder::DFABuilder(std::shared_ptr<NFA> nfa)
    : impl_(new Impl)
{
    if(nfa == nullptr){
        return;
    }
    impl_->nfa = nfa;
}

DFABuilder::~DFABuilder() = default;

std::shared_ptr<DFA>  DFABuilder::build()
{
    if(impl_->temp != nullptr){
        impl_->temp.reset();
    }
    impl_->temp = std::make_shared<Impl::Temp>();

    std::shared_ptr<DFA>  dfa = std::make_shared<DFA>();
    if(dfa == nullptr){
        return nullptr;
    }

    dfa->start = impl_->CreateDFANode(dfa);
    std::set<unsigned char> nfaState = epsilonClosure(impl_->nfa->start);
    if(nfaState.count(impl_->nfa->accept->state) > 0){
        dfa->start->isAcceptState = true;
    }
    impl_->addNewState(dfa->start, nfaState);

    while(true){
        unsigned char markcount = 0;
        auto tempState = impl_->temp->states;
        for (auto &nodeInfo : tempState)
        {
            FANode *currentNode = nodeInfo->fanode;
            std::set<unsigned char> currentNfaState = nodeInfo->nfaState;
            
            if(nodeInfo->isMark){
                markcount++;
                continue;
            }else{
                nodeInfo->isMark = true;
                markcount++;
                            
                for (FASymbol alpha : impl_->nfa->alphabet)
                {
                    nfaState = epsilonClosure(move(currentNfaState, alpha));
                    if(nfaState.empty()){
                        continue;
                    }
                    if(!impl_->alreadyInStates(nfaState)){
                        FANode *newNode = impl_->CreateDFANode(dfa);
                        dfa->stateSet[newNode->state] = newNode;
                        if(nfaState.count(impl_->nfa->accept->state) > 0){
                            newNode->isAcceptState = true;
                        }
                        impl_->addNewState(newNode, nfaState);
                        currentNode->edges.push_back({newNode, alpha});
                    } else{
                        FANode *node = impl_->findNodeByStateSet(nfaState);
                        assert(node != nullptr);
                        currentNode->edges.push_back({node, alpha});
                    }
                }
            }
        }
        if(markcount == impl_->temp->states.size()){
            break;
        }
    }
    if(!impl_->temp->states.empty()){
        dfa->accept = impl_->temp->states[impl_->temp->states.size() - 1]->fanode;
    }
    return dfa;
}

std::set<unsigned char> DFABuilder::epsilonClosure(FANode *node)
{
    std::set<unsigned char> states;
    if (node == nullptr)
    {
        return states;
    }
    impl_->epsilonClosure(node, states);
    return states;
}

std::set<unsigned char> DFABuilder::epsilonClosure(const std::set<unsigned char> &states)
{
    std::set<unsigned char> retStates;
    for (unsigned char state : states)
    {
        FANode *node = impl_->nfa->stateSet[state];
        if(node == nullptr){
            break;
        }
        std::set<unsigned char> set = epsilonClosure(node);
        retStates.insert(set.begin(), set.end());
    }
    return retStates;
}

std::set<unsigned char> DFABuilder::move(std::set<unsigned char> &states, FASymbol c)
{
    std::set<unsigned char> retStates;
    for (unsigned char state : states)
    {
        FANode *node = impl_->nfa->stateSet[state];
        if(node == nullptr){
            return {};
        }
        for (size_t i = 0; i < node->edges.size(); i++)
        {
            if(node->edges[i].lable == c){
                retStates.insert(node->edges[i].destination->state);
            }
        } 
    }
    return retStates;
}
