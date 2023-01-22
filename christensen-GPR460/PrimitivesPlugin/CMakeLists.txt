cmake_minimum_required (VERSION 3.8)

project ("PrimitivesPlugin")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" PrimitivesPlugin_sources)
add_library("PrimitivesPlugin" SHARED ${PrimitivesPlugin_sources}) # FIXME make module?
target_include_directories("PrimitivesPlugin" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_link_libraries("PrimitivesPlugin" "engine-core")

# Declare exports
target_include_directories("PrimitivesPlugin" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
set_target_properties("PrimitivesPlugin" PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/PrimitivesPlugin"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/PrimitivesPlugin"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/PrimitivesPlugin"
)