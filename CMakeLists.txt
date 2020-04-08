cmake_minimum_required(VERSION 3.5)

project(src LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "-O3")

add_executable(src main.cpp)
