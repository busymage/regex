cmake_minimum_required(VERSION 3.0.0)
find_package(GTest REQUIRED)

add_executable(CompilerTest src/CompilerTest.cpp)
add_executable(DFABuilderTest src/DFABuilderTest.cpp)
add_executable(NFABuilderTest src/NFABuilderTest.cpp)


target_link_libraries(CompilerTest PUBLIC
    GTest::GTest
    GTest::Main
    Compiler)

target_link_libraries(DFABuilderTest PUBLIC
    GTest::GTest
    GTest::Main
    Compiler)

target_link_libraries(NFABuilderTest PUBLIC
    GTest::GTest
    GTest::Main
    Compiler)

add_test(
    NAME CompilerTest
    COMMAND CompilerTest)

add_test(
    NAME DFABuilderTest
    COMMAND DFABuilderTest)

add_test(
    NAME NFABuilderTest
    COMMAND NFABuilderTest)

