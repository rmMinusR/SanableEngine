cmake_minimum_required (VERSION 3.8)

project ("PluginControlUI")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" PluginControlUI_sources)
declare_plugin("PluginControlUI" PluginControlUI_sources)
target_include_directories("PluginControlUI" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_link_libraries("PluginControlUI" "engine-core")

# Declare exports
target_include_directories("PluginControlUI" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")

# Declare exported resources
file(GLOB_RECURSE FOUND_RESOURCES
	LIST_DIRECTORIES false
	RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
	"${CMAKE_CURRENT_LIST_DIR}/resources/**"
)
foreach(RES ${FOUND_RESOURCES})
	export_resource(PluginControlUI "${RES}")
endforeach()
