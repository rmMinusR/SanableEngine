cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

project("engine-graphics")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_graphics_sources)
declare_plugin("engine-graphics" "" engine_graphics_sources)
target_include_directories("engine-graphics" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_include_directories("engine-graphics" PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries("engine-graphics" PUBLIC ${SDL2_LIBRARIES} engine-memory)

# Declare exports
target_include_directories("engine-graphics" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
