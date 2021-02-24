cmake_minimum_required(VERSION 3.0.0)

find_package(GTest REQUIRED)

set(Sources src/EngineTest.cpp)

add_executable(EngineTest ${Sources})

target_link_libraries(EngineTest PUBLIC
    GTest::GTest
    GTest::Main
    Engine)

add_test(
    NAME EngineTest
    COMMAND EngineTest)

