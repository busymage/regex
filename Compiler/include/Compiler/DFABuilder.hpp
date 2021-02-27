#ifndef REGEX_DFABUILDER_HPP
#define REGEX_DFABUILDER_HPP

#include <memory>
#include <set>

struct DFA  ;
struct NFA;
struct FANode;

class DFABuilder{
    public:
    DFABuilder(std::shared_ptr<NFA> nfa);

    ~DFABuilder();

    std::shared_ptr<DFA> build();

    std::set<unsigned char> epsilonClosure(FANode *node);

    std::set<unsigned char> epsilonClosure(const std::set<unsigned char> &states);

    std::set<unsigned char> move(std::set<unsigned char> &states, FASymbol c);

    private:
        struct Impl;

        std::unique_ptr<Impl> impl_;
};

#endif