cmake_minimum_required(VERSION 3.20.0)
project(cppproject VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
