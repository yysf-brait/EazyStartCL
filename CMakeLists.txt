cmake_minimum_required(VERSION 3.28)
project(EazyStartCL C CXX)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.c)

add_subdirectory(EazyStart)
target_link_libraries(${PROJECT_NAME} PRIVATE EazyStart)
