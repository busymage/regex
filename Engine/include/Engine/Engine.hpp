#ifndef REGEX_ENGINE_HPP
#define REGEX_ENGINE_HPP

#include <memory>

class Engine{
    public:
    Engine(const std::string &regexString);

    ~Engine();

    bool accept(const std::string &match);

    private:
        struct Impl;

        std::unique_ptr<Impl> impl_;
};

#endif