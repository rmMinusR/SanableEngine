cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

add_library(doctest "${CMAKE_CURRENT_LIST_DIR}/dummy.cpp")

target_include_directories(doctest PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
