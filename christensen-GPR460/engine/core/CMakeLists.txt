cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

project("engine-core")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_core_sources)
add_library("engine-core" SHARED ${engine_core_sources})
target_include_directories("engine-core" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_include_directories("engine-core" PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries("engine-core" ${SDL2_LIBRARIES})

# Declare exports
target_include_directories("engine-core" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
