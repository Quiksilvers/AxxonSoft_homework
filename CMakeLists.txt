cmake_minimum_required(VERSION 3.5.1)

project(linecounter)

set(EXEC linecounter)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES src/*.cpp)

add_executable(${EXEC} ${SOURCES})