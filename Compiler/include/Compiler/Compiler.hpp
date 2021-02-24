#ifndef REGEX_COMPILER_HPP
#define REGEX_COMPILER_HPP

#include <memory>
#include <string>

struct DFA;

class Compiler{
    public:
    Compiler();

    ~Compiler();

    DFA *compile(const std::string &input);

    private:
        struct Impl;

        std::unique_ptr<Impl> impl_;
};


#endif