cmake_minimum_required (VERSION 3.8)

project ("UserGameplayPlugin")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" UserGameplayPlugin_sources)
add_library("UserGameplayPlugin" SHARED ${UserGameplayPlugin_sources})
target_include_directories("UserGameplayPlugin" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_link_libraries("UserGameplayPlugin" "PrimitivesPlugin")
target_link_libraries("UserGameplayPlugin" "engine-core")

# Declare exports
target_include_directories("UserGameplayPlugin" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
set_target_properties("UserGameplayPlugin" PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/UserGameplayPlugin"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/UserGameplayPlugin"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/UserGameplayPlugin"
)
