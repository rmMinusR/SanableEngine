cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

project("engine-serialization")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_serialization_sources)
add_library("engine-serialization" SHARED ${engine_serialization_sources})
target_include_directories("engine-serialization" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_include_directories("engine-serialization" PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries("engine-serialization" ${SDL2_LIBRARIES})

# Declare exports
target_include_directories("engine-serialization" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
