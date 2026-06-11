cmake_minimum_required(VERSION 3.15)
project(Q3)
set(CMAKE_CXX_STANDARD 17)

find_library(OSQP_LIB osqp PATHS /usr/local/lib)
if(NOT OSQP_LIB)
    message(FATAL_ERROR "未找到系统安装的OSQP库，请执行 sudo ldconfig 刷新缓存")
endif()

add_executable(Q3 q3.cpp)
target_link_libraries(Q3 ${OSQP_LIB})