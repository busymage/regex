#ifndef REGEX_NFABUILDER_HPP
#define REGEX_NFABUILDER_HPP

#include <memory>

struct NFA;
struct AST;

class NFABuilder{
    public:
    NFABuilder();

    ~NFABuilder();

    std::shared_ptr<NFA> fromAST(std::shared_ptr<AST>ast);

    private:
        struct Impl;

        std::unique_ptr<Impl> impl_;
};

#endif