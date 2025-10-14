cmake_minimum_required(VERSION 3.20.0)

# Catch2 unit test integration
find_package(Catch2 3 REQUIRED)

add_executable(test_lexer test_lexer.cpp ../src/lexer.cpp ../src/file.cpp)
target_link_libraries(test_lexer PRIVATE Catch2::Catch2WithMain)
include_directories(../src)

set_property(TARGET test_lexer PROPERTY CXX_STANDARD 23)

include(CTest)
include(Catch)
catch_discover_tests(test_lexer)