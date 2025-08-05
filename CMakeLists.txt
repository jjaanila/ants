cmake_minimum_required(VERSION 3.10)
project(Ants)

set(CMAKE_CXX_STANDARD 17)

add_executable(ants ./src/main.cpp ./src/Ant.cpp ./src/Id.cpp)