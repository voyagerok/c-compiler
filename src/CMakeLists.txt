cmake_minimum_required(VERSION 3.20.0)

add_executable(cppproject main.cpp
        lexer.cpp
        lexer.h
        file.cpp
        file.h)
set_property(TARGET cppproject PROPERTY CXX_STANDARD 23)