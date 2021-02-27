#ifndef REGEX_PARSER_HPP
#define REGEX_PARSER_HPP

#include <memory>
#include <string>

struct AST;

class Parser{
    public:
    Parser(const std::string &input);

    ~Parser();

    std::shared_ptr<AST>parse();

    private:
        struct Impl;

        std::unique_ptr<Impl> impl_;
};

#endif