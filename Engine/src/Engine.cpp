#include <CommonDataStructure/FA.hpp>
#include <Compiler/Compiler.hpp>
#include <Engine/Engine.hpp>

struct Engine::Impl{
    std::string regexString;
   std::unique_ptr<DFA> dfa;

   bool transaction(unsigned char &currentState, char c)
   {
        FANode *node = dfa->stateSet[currentState];
        if(node == nullptr){
           return false;
        }
        for (size_t i = 0; i < node->edges.size(); i++)
        {
            if(c == node->edges[i].lable){
                currentState = node->edges[i].destination->state;
                return true;
            }
        }
        return false;
   }
};

Engine::Engine(const std::string &regexString)
    :impl_(new Impl)
{
    impl_->regexString = regexString;
    Compiler compiler;
    DFA *dfa = compiler.compile(regexString);
    impl_->dfa = std::unique_ptr<DFA>(dfa);
}

bool Engine::accept(const std::string &match)
{
    unsigned char currentState = impl_->dfa->initalState;
    for (size_t i = 0; i < match.length(); i++)
    {
        char c = match[i];
        if(!impl_->transaction(currentState, c)){
            return false;
        }
    }
    return impl_->dfa->stateSet[currentState]->isAcceptState;
}

Engine::~Engine() = default;

