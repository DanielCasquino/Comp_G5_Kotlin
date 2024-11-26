cmake_minimum_required(VERSION 3.28)
project(Scanner)

set(CMAKE_CXX_STANDARD 23)

add_executable(Scanner main.cpp exp.cpp exp.h
      parser.cpp parser.h environment.hh imp_type_checker.cpp imp_type_checker.hh
     scanner.cpp scanner.h Token.cpp token.h  Token.hpp
        imp_interpreter.cpp imp_interpreter.hh
   imp_value.cpp imp_value.hh imp_value_visitor.hh
   imp_type.cpp imp_type.hh
    TokenTrie.hpp TokenTrie.cpp
type_visitor.h
    visitor.cpp visitor.h
    imp_codegen.cpp imp_codegen.hh)
add_executable( pila svm.cpp svm_parser.cpp svm_run.cpp)
