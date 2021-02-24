cmake_minimum_required(VERSION 3.0.0)

set(Sources src/Compiler.cpp
            src/DFABuilder.cpp
            src/NFABuilder.cpp)

set(Headers include/Compiler/Compiler.hpp
            include/Compiler/DFABuilder.hpp
            include/Compiler/NFABuilder.hpp)

add_library(Compiler ${Sources} ${Headers})

include_directories(include
    ../CommonDataStructure/include
    ../Scanner/include
    ../Parser/include)

target_link_libraries(Compiler
    Scanner
    Parser)

add_subdirectory(test)