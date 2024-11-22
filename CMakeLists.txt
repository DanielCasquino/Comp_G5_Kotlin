cmake_minimum_required(VERSION 3.28)
project(Scanner)

set(CMAKE_CXX_STANDARD 23)

add_executable(Scanner main.cpp exp.cpp exp.hpp scanner.cpp scanner.hpp token.cpp token.hpp visitor.cpp visitor.hpp tokentrie.cpp tokentrie.hpp)
