#ifndef REGEX_ENGINE_HPP
#define REGEX_ENGINE_HPP

#include <memory>
#include <string>
#include <vector>

class Engine{
    public:
    Engine(const std::string &regexString);

    ~Engine();

    bool accept(const std::string &input);

    std::vector<std::string> match(const std::string &input);

    private:
        struct Impl;

        std::unique_ptr<Impl> impl_;
};

#endif