cmake_minimum_required(VERSION 3.20.0)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

if ("cxx_std_23" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
  target_compile_features(Catch2 PRIVATE cxx_std_23)
  target_compile_features(Catch2WithMain PRIVATE cxx_std_23)
endif()

add_executable(test_lexer test_lexer.cpp ../src/lexer.cpp ../src/file.cpp)
target_link_libraries(test_lexer PRIVATE Catch2::Catch2WithMain)
include_directories(../src)

set_property(TARGET test_lexer PROPERTY CXX_STANDARD 23)

include(CTest)
include(Catch)
catch_discover_tests(test_lexer)