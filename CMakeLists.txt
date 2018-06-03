cmake_minimum_required(VERSION 3.10)
project(big_integer_optimized)

include_directories(${big_integer_optimized_SOURCE_DIR})

add_executable(big_integer_testing
        utils.h
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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG -g")
endif()

target_link_libraries(big_integer_testing -lgmp -lgmpxx -lpthread)



set(CMAKE_CXX_STANDARD 17)

add_executable(big_integer_optimized
        vector_digit_testing.cpp
        big_integer.cpp
        big_integer.h
        utils.h
        long_arithmetic.cpp
        vector_digit.h
        vector_digit.cpp
        vector_digit_testing.cpp)


add_executable(vector_digit_testing
        vector_digit_testing.cpp
        vector_digit.cpp
        vector_digit.h
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc)


target_link_libraries(vector_digit_testing -lgmp -lgmpxx -lpthread)