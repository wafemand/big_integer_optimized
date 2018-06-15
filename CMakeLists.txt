cmake_minimum_required(VERSION 3.10)
project(big_integer_optimized)

set(CMAKE_CXX_STANDARD 17)

#sudo perf record -e instructions -c 1000000 --call-graph dwarf ./big_integer_testing

include_directories(${big_integer_optimized_SOURCE_DIR})

add_executable(big_integer_testing
        big_integer.h
        big_integer.cpp
        long_arithmetic.cpp
        big_integer_testing.cpp
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc
        vector_digit.h
        vector_digit.cpp)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -O2")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")-Wall -fprofile-arcs -ftest-coverage -fsanitize=address,undefined
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -g -O0 -fno-omit-frame-pointer -fsanitize=address,undefined")
endif()

target_link_libraries(big_integer_testing -lgmp -lgmpxx -lpthread)



