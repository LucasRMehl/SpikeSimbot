cmake_minimum_required (VERSION 2.8.7)

set(PROJECT_NAME SpecialKEval)
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "-g -Wall")

### Build
add_library(specialkeval src/FiveEval.cpp src/SevenEval.cpp)
