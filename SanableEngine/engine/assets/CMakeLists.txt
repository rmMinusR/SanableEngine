cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

project("engine-assets")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_assets_sources)
declare_plugin("engine-assets" "" engine_assets_sources)
target_include_directories("engine-assets" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_include_directories("engine-assets" PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries("engine-assets" ${SDL2_LIBRARIES})

# Declare exports
target_include_directories("engine-assets" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
