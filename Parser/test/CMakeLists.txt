cmake_minimum_required(VERSION 3.0.0)

find_package(GTest REQUIRED)

set(Sources src/ParserTest.cpp)

add_executable(ParserTest ${Sources})

target_link_libraries(ParserTest PUBLIC
    GTest::GTest
    GTest::Main
    Parser)

add_test(
    NAME ParserTest
    COMMAND ParserTest)

