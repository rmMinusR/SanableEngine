cmake_minimum_required (VERSION 3.13)

add_library(stb_image STATIC "${CMAKE_CURRENT_LIST_DIR}/stb_image.cpp")
target_include_directories(stb_image PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")