cmake_minimum_required(VERSION 3.14)
project(q1_grad)
set(CMAKE_CXX_STANDARD 17)

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

add_executable(q1 Q1.cpp)