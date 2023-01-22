cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("engine")

include("${CMAKE_CURRENT_LIST_DIR}/core/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/vendor/CMakeLists.txt")
