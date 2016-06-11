cmake_minimum_required(VERSION 3.5)
project(HermansLinearHashtable)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 --std=c++11 --pedantic-errors -DETYPE=std::size_t -DSIZE=2")

set(SOURCE_FILES simpletest.cpp Container.h HermansLinearHashtable.h)
add_executable(HermansLinearHashtable ${SOURCE_FILES})