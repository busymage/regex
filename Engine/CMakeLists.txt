cmake_minimum_required(VERSION 3.0.0)

set(Sources src/Engine.cpp)

set(Headers include/Engine/Engine.hpp)

include_directories(include
    ../CommonDataStructure/include
    ../Compiler/include)

add_library(Engine ${Sources} ${Headers})

target_link_libraries(Engine
    Compiler)

add_subdirectory(test)