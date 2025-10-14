cmake_minimum_required(VERSION 3.20.0)
project(cppproject VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(src)
add_subdirectory(tests)

# add_executable(cppproject src/main.cpp
#         src/lexer.cpp
#         src/lexer.h
#         src/file.cpp
#         src/file.h)
# set_property(TARGET cppproject PROPERTY CXX_STANDARD 23)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
