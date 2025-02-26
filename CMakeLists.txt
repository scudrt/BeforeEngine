cmake_minimum_required(VERSION 3.26)

project (BeforeEngine CXX)

add_executable(main ${CMAKE_SOURCE_DIR}/source/main.cpp)


target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/source)
