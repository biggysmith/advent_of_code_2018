cmake_minimum_required(VERSION 3.5.1)

project(advent_of_code_2018)

set(CMAKE_CXX_STANDARD 17)

add_executable(day01 src/day01/day1.cpp)
add_executable(day02 src/day02/day2.cpp)
add_executable(day03 src/day03/day3.cpp)
add_executable(day04 src/day04/day4.cpp)
add_executable(day05 src/day05/day5.cpp)
add_executable(day06 src/day06/day6.cpp)
add_executable(day07 src/day07/day7.cpp)
add_executable(day08 src/day08/day8.cpp)
add_executable(day09 src/day09/day9.cpp)
add_executable(day10 src/day10/day10.cpp)
add_executable(day11 src/day11/day11.cpp)
add_executable(day12 src/day12/day12.cpp)
add_executable(day13 src/day13/day13.cpp)
add_executable(day14 src/day14/day14.cpp)

set(CMAKE_CXX_STACK_SIZE "10000000") #recursion!!!

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}") 
endif()

