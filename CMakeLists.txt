cmake_minimum_required(VERSION 3.17)
project(code-optimization LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)
include_directories(lib)

# Add tracy profiler to all targets
add_library(TRACY lib/tracy/TracyClient.cpp)
target_include_directories(TRACY SYSTEM PUBLIC lib/tracy)
add_compile_definitions(TRACY_ENABLE)

add_subdirectory(src)
