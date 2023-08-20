cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

project("engine-rtti-test")

aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_rtti_test_sources)
add_executable("engine-rtti-test" ${engine_rtti_test_sources})
target_include_directories("engine-rtti-test" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

target_link_libraries("engine-rtti-test" PUBLIC engine-rtti engine-rtti-test-target)
