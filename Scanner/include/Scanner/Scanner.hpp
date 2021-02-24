#ifndef REGEX_SCANNER_HPP
#define REGEX_SCANNER_HPP

#include <CommonDataStructure/Token.hpp>
#include <memory>
#include <string>
#include <vector>

class Scanner{
    public:
    Scanner(const std::string &input);

    ~Scanner();

    Token getNextToken();

    std::vector<Token> getNextTokens(int num);

    Token peekNextToken();

    std::vector<Token> peekNextTokens(int num);

    private:
        struct Impl;

        std::unique_ptr<Impl> impl_;
};

#endif