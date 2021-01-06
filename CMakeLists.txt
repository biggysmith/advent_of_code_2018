cmake_minimum_required(VERSION 3.5.1)

project(advent_of_code_2018)

set(CMAKE_CXX_STANDARD 17)

add_executable(day01 src/day01/day1.cpp)
add_executable(day02 src/day02/day2.cpp)
add_executable(day03 src/day03/day3.cpp)

set(CMAKE_CXX_STACK_SIZE "10000000") #recursion!!!

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}") 
endif()

