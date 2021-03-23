project(tp-2)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX TRACY ${OpenCV_LIBS})
