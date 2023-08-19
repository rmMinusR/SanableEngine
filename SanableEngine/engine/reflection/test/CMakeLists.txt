cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

project("engine-rtti-test")

aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/main/src" engine_rtti_test_sources)
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/framework/src" engine_rtti_test_framework_sources)
add_executable("engine-rtti-test" ${engine_rtti_test_sources} ${engine_rtti_test_framework_sources})
target_include_directories("engine-rtti-test" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/main/include")
target_include_directories("engine-rtti-test" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/framework/include")

target_link_libraries("engine-rtti-test" PUBLIC engine-rtti)

# Custom reflection command
generate_reflection(engine-rtti-test "${CMAKE_CURRENT_LIST_DIR}/main")
