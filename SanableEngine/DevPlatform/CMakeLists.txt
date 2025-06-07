cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" DevPlatform_sources)
add_sanable_executable(DevPlatform "${DevPlatform_sources}")
