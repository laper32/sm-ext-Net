cmake_minimum_required(VERSION 3.22)

project(sm-ext-Net)

include(${CMAKE_CURRENT_LIST_DIR}/configure.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/All.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(src)