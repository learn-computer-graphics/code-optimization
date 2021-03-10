cmake_minimum_required(VERSION 3.17)
project(code-optimization LANGUAGES C CXX)

find_package(OpenMP REQUIRED)
include_directories(lib)

add_subdirectory(src)
