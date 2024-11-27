cmake_minimum_required(VERSION 3.28)
project(Scanner)

set(CMAKE_CXX_STANDARD 23)

add_executable(Scanner main.cpp exp.cpp exp.hpp
      parser.cpp parser.hpp environment.hh imp_type_checker.cpp imp_type_checker.hh
     scanner.cpp scanner.hpp token.cpp token.hpp
        imp_interpreter.cpp imp_interpreter.hh
   imp_value.cpp imp_value.hh imp_value_visitor.hh
   imp_type.cpp imp_type.hh
type_visitor.hpp
TokenTrie.cpp TokenTrie.hpp
    visitor.cpp visitor.hpp
    imp_codegen.cpp imp_codegen.hh)
add_executable( pila svm.cpp svm_parser.cpp svm_run.cpp)
