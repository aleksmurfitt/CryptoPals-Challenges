cmake_minimum_required(VERSION 3.15)

project(cryptopals)

set(CMAKE_CXX_STANDARD 20)

add_executable(cryptopals src/main.cpp)
target_include_directories(cryptopals PRIVATE include)