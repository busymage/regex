#include <CommonDataStructure/AST.hpp>
#include <CommonDataStructure/FA.hpp>
#include <Compiler/Compiler.hpp>
#include <Compiler/DFABuilder.hpp>
#include <Compiler/NFABuilder.hpp>
#include <Parser/Parser.hpp>
#include <string>


struct Compiler::Impl{
};

Compiler::Compiler()
:impl_(new Impl)
{
}

Compiler::~Compiler() = default;

DFA *Compiler::compile(const std::string &input)
{
    AST *ast = nullptr;

    if (input.length() != 0)
    {
        Parser parser(input);
        ast = parser.parse();
        if (ast == nullptr)
        {
            return nullptr;
        }
    }

    NFABuilder nfaBuilder;
    NFA *nfa = nfaBuilder.fromAST(ast);
    if (nfa == nullptr)
    {
        return nullptr;
    }

    DFABuilder dfaBuilder(nfa);
    DFA *dfa = dfaBuilder.build();
    return dfa;
}