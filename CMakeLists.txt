cmake_minimum_required(VERSION 3.0.0)
project(Regex VERSION 0.1.0)

enable_testing()

add_subdirectory(Compiler)
add_subdirectory(Engine)
add_subdirectory(Scanner)
add_subdirectory(Parser)