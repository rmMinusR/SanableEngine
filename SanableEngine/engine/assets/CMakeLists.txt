cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

project("engine-assets")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_assets_sources)
declare_dll("engine-assets" "" engine_assets_sources)
target_include_directories("engine-assets" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_include_directories("engine-assets" PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries("engine-assets" PUBLIC ${SDL2_LIBRARIES} engine-memory)

# Declare exports
target_include_directories("engine-assets" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")

# Declare install targets
install(TARGETS "engine-assets")
