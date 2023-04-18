cmake_minimum_required (VERSION 3.8)

project ("PrimitivesPlugin")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" PrimitivesPlugin_sources)
declare_plugin("PrimitivesPlugin" PrimitivesPlugin_sources)
target_include_directories("PrimitivesPlugin" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_link_libraries("PrimitivesPlugin" "engine-core")

# Declare exports
target_include_directories("PrimitivesPlugin" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
