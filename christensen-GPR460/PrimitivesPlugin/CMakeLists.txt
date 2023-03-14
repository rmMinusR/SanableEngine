cmake_minimum_required (VERSION 3.8)

project ("PrimitivesPlugin")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" PrimitivesPlugin_sources)
add_library("PrimitivesPlugin" SHARED ${PrimitivesPlugin_sources}) # FIXME make module?
set_linkage_shared(PrimitivesPlugin)
target_include_directories("PrimitivesPlugin" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Fix output
if(EMSCRIPTEN)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		PREFIX ""
		SUFFIX "so"
	)
endif()

# Declare imports
target_link_libraries("PrimitivesPlugin" "engine-core")

# Declare exports
target_include_directories("PrimitivesPlugin" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
export_plugin_dll(PrimitivesPlugin)
