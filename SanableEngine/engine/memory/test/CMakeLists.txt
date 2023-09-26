cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

project("engine-memory-test")

aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_memory_test_sources)
add_executable("engine-memory-test" ${engine_memory_test_sources})

target_include_directories("engine-memory-test" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")

target_link_libraries("engine-memory-test" PUBLIC doctest engine-memory)
