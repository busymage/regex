cmake_minimum_required(VERSION 3.0.0)

set(Sources src/Scanner.cpp)

set(Headers include/Scanner/Scanner.hpp)

include_directories(include
    ../CommonDataStructure/include)

add_library(Scanner ${Sources} ${Headers})

target_link_libraries(Scanner)

add_subdirectory(test)