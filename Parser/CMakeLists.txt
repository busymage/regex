cmake_minimum_required(VERSION 3.0.0)
set(Sources src/Parser.cpp)

set(Headers include/Parser/Parser.hpp)

add_library(Parser ${Sources} ${Headers})
target_link_libraries(Parser Scanner)

include_directories(include ../Scanner/include
    ../CommonDataStructure/include)

add_subdirectory(test)