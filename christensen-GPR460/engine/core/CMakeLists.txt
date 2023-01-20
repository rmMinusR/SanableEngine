cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("engine-core")

# Declare target
aux_source_directory("core/src" engine_core_sources)
add_library("engine-core" STATIC ${engine_core_sources}) #TODO switch to SHARED?
target_include_directories("engine-core" PRIVATE "core/private")

# Declare imports
include_directories("engine-core" PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries("engine-core" ${SDL2_LIBRARIES})

# Declare exports
target_include_directories("engine-core" PUBLIC "core/public")
