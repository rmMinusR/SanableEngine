cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

project("engine-common")

add_library("engine-common" STATIC "${CMAKE_CURRENT_LIST_DIR}/dummy.cpp")
target_include_directories("engine-common" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
target_link_libraries("engine-common" glm::glm)
