#include <Compiler/FA.hpp>
#include <Compiler/Compiler.hpp>
#include <Engine/Engine.hpp>
#include <sstream>
#include <string>

std::string getLineFromPos(const std::string &str, size_t pos)
{
    size_t lineEnd = str.find('\n', pos);
    if(std::string::npos == lineEnd){
        lineEnd = str.length();
    }
    std::string result =  str.substr(pos, lineEnd - pos);
    return result;
}

FANode *findMostMatchDestination(FANode *node, char c)
   {
       FANode *dest = nullptr;
       size_t symbolRange = 0;
       for (size_t i = 0; i < node->edges.size(); i++)
        {
            char lower = node->edges[i].lable.lowerBound;
            char upper = node->edges[i].lable.upperBound;
            if(c >= lower && c <= upper){
                if(dest != nullptr){
                    if(upper - lower < symbolRange){
                        dest = node->edges[i].destination;
                        symbolRange = upper - lower;
                        if(symbolRange == 0){
                            return dest;
                        }
                    }
                }else{
                    dest = node->edges[i].destination;
                    symbolRange = upper - lower;
                    if(symbolRange == 0){
                            return dest;
                    }
                }
            }
        }
        return dest;
   }

struct Engine::Impl{
    std::string regexString;
    std::shared_ptr<DFA> dfa;

   bool hasStartAnchor = false;
   bool hasEndAnchor = false;

   bool isAcceptState(FAState state)
   {
       return dfa->stateSet[state]->isAcceptState;
   }

   bool transaction(unsigned char &currentState, char c)
   {
        FANode *node = dfa->stateSet[currentState];
        if(node == nullptr){
           return false;
        }
        FANode *dest = findMostMatchDestination(node, c);
        if(dest == nullptr){
            return false;
        } else{
            currentState = dest->state;
            return true;
        }
   }

   bool acceptUntil(const std::string &input, size_t start, size_t &end)
   {
       size_t pos = start;
       FAState currentState = dfa->start->state;
       for(pos; pos < input.length(); pos++)
       {
           char c = input[pos];
           if(!transaction(currentState, c)){
               if(isAcceptState(currentState)){
                   end = pos;
                   return true;
               } else{
                   //occur error and not accept
                   end = input.length();
                   return false;
               }
           }
       }
       //reach end of input
       end = input.length();
       if(!isAcceptState(currentState)){
           return false;
       }else{
           return true;
       }
   }

   std::vector<std::string> matchAnyWhere(const std::string &input)
   {
       std::vector<std::string> result;
       size_t curPos = 0;
       while(curPos < input.length()){
           size_t end = 0;
           if(acceptUntil(input, curPos ,end)){
               result.push_back(input.substr(curPos, end - curPos));
               curPos = end;
           }else{
               curPos++;
           }
       }
       return result;
   }

   std::vector<std::string> matchLineEnd(const std::string &input)
   {
       std::vector<std::string> result;
       size_t curPos = 0;
       while (curPos < input.length())
       {
           std::string line;
           size_t end = 0;
           line = getLineFromPos(input, curPos);
           if (acceptUntil(line, 0, end))
           {
               if (end == line.length())
               {
                   result.emplace_back(line.begin(), line.end());
                   curPos += line.length();
                   continue;
               }
               else
               {
                   curPos++;
               }
           }
           else
           {
               curPos++;
           }
       }
       return result;
   }
};

Engine::Engine(const std::string &regexString)
    :impl_(new Impl)
{
    std::string regex = regexString;
    if(regexString.length() >=2){
        if(regexString[0] == '^'){
            regex = regex.substr(1);
            impl_->hasStartAnchor = true;
        }
        if(regexString[regexString.length() - 1] == '$'){
            regex = regex.substr(0, regex.length() - 1);
            impl_->hasEndAnchor = true;
        }
    }
    impl_->regexString = regexString;
    Compiler compiler;
    impl_->dfa = compiler.compile(regex);
}

bool Engine::accept(const std::string &input)
{
    unsigned char currentState = impl_->dfa->start->state;
    for (size_t i = 0; i < input.length(); i++)
    {
        char c = input[i];
        if(!impl_->transaction(currentState, c)){
            return false;
        }
    }
    return impl_->dfa->stateSet[currentState]->isAcceptState;
}

std::vector<std::string> Engine::match(const std::string &input)
{
    std::vector<std::string> result;
    if(impl_->hasStartAnchor){
        std::istringstream iss(input);
        std::string line;
        while(std::getline(iss, line))
        {
            size_t currentPos = 0;
            size_t endPos = 0;
            if(impl_->acceptUntil(line, currentPos, endPos)){
                if(impl_->hasEndAnchor == true){
                    if(endPos == line.length()){
                        result.emplace_back(line.begin(), line.end());
                    }
                }else{
                    result.emplace_back(line.begin(), line.begin() + endPos);
                }
            }
        }
    }
    else
    {
        if(impl_->hasEndAnchor){
            result = impl_->matchLineEnd(input);
        }
        else{
            result = impl_->matchAnyWhere(input);
        }
    }

    return result;
}

Engine::~Engine() = default;

