#ifndef REGEX_DFABUILDER_HPP
#define REGEX_DFABUILDER_HPP

#include <memory>
#include <set>

struct DFA;
struct NFA;
struct FANode;

class DFABuilder{
    public:
    DFABuilder(NFA *nfa);

    ~DFABuilder();

    DFA *build();

    std::set<unsigned char> epsilonClosure(FANode *node);

    std::set<unsigned char> epsilonClosure(const std::set<unsigned char> &states);

    std::set<unsigned char> move(std::set<unsigned char> &states, char c);

    private:
        struct Impl;

        std::unique_ptr<Impl> impl_;
};

#endif