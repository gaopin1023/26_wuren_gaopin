cmake_minimum_required(VERSION 3.19)
project(Test)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  

find_package(OpenCV 4.5 REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_subdirectory(common)
add_subdirectory(modules)

include_directories(
    ${CMAKE_SOURCE_DIR}/modules/M1/include
    ${CMAKE_SOURCE_DIR}/modules/A1/include
    ${CMAKE_SOURCE_DIR}/modules/A2/include
    ${CMAKE_SOURCE_DIR}/modules/M2/include
    ${CMAKE_SOURCE_DIR}/common/math/include
    ${CMAKE_SOURCE_DIR}/common/kalman/include
)

add_library(A1 STATIC
    ${CMAKE_SOURCE_DIR}/modules/A1/src/A11.cpp
    ${CMAKE_SOURCE_DIR}/modules/A1/src/A12.cpp
    ${CMAKE_SOURCE_DIR}/modules/A1/src/A13.cpp
)
target_include_directories(A1 PUBLIC ${CMAKE_SOURCE_DIR}/modules/A1/include)

add_library(A2 STATIC
    ${CMAKE_SOURCE_DIR}/modules/A2/src/A2.cpp
)
target_include_directories(A2 PUBLIC ${CMAKE_SOURCE_DIR}/modules/A2/include)

add_library(M2 STATIC
    ${CMAKE_SOURCE_DIR}/modules/M2/src/M2.cpp
)
target_include_directories(M2 PUBLIC ${CMAKE_SOURCE_DIR}/modules/M2/include)
target_link_libraries(M2 PUBLIC A1 A2)

add_library(math STATIC
    ${CMAKE_SOURCE_DIR}/common/math/src/Math.cpp
)
target_include_directories(math PUBLIC ${CMAKE_SOURCE_DIR}/common/math/include)
target_link_libraries(math PUBLIC ${OpenCV_LIBS})

add_executable(test main.cpp)
target_link_libraries(test PUBLIC kalman math A1 A2 M1 M2)