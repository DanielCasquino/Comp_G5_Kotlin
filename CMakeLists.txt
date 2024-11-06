cmake_minimum_required(VERSION 3.28)
project(Scanner)

set(CMAKE_CXX_STANDARD 17)

add_executable(Scanner main.cpp exp.cpp exp.h parser.cpp parser.h scanner.cpp scanner.h token.cpp token.h visitor.cpp visitor.h)
