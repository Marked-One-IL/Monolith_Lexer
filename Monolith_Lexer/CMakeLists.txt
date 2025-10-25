cmake_minimum_required(VERSION 4.0)
project(Project)

set(CMAKE_CXX_STANDARD 26)

add_executable(Project main.cpp
        Lexer/Generator.cpp
        Lexer/Generator.cpp
        Lexer/Token.cpp
        Helper/Helper.cpp
        Helper/Assert.hpp)
