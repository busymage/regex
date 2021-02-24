cmake_minimum_required(VERSION 3.0.0)

find_package(GTest REQUIRED)

set(Sources src/ScannerTest.cpp)

add_executable(ScannerTest ${Sources})

target_link_libraries(ScannerTest PUBLIC
    GTest::GTest
    GTest::Main
    Scanner)

add_test(
    NAME ScannerTest
    COMMAND ScannerTest)

