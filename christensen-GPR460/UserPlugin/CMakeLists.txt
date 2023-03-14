cmake_minimum_required (VERSION 3.8)

project ("UserGameplayPlugin")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" UserGameplayPlugin_sources)
add_library("UserGameplayPlugin" SHARED ${UserGameplayPlugin_sources})
set_linkage_shared(UserGameplayPlugin)
target_include_directories("UserGameplayPlugin" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Fix output
if(EMSCRIPTEN)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		PREFIX ""
		SUFFIX ".so"
	)
endif()

# Declare imports
target_link_libraries("UserGameplayPlugin" "PrimitivesPlugin")
target_link_libraries("UserGameplayPlugin" "engine-core")

# Declare exports
target_include_directories("UserGameplayPlugin" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
export_plugin_dll(UserGameplayPlugin)
