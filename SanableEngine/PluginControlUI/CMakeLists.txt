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
export_plugin_resource(PluginControlUI "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/button/normal.png)
export_plugin_resource(PluginControlUI "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/button/normal_pressed.png)
export_plugin_resource(PluginControlUI "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/button/disabled.png)
export_plugin_resource(PluginControlUI "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/button/disabled_pressed.png)
export_plugin_resource(PluginControlUI "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/field.png)
export_plugin_resource(PluginControlUI "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/parent.png)
export_plugin_resource(PluginControlUI "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/fonts/arial.ttf)

# Export dev manifest
option(ENABLE_DEV_ENV "Enable development environment integrations" TRUE)
if(ENABLE_DEV_ENV)
	get_target_property(installLoc PluginControlUI RUNTIME_OUTPUT_DIRECTORY)
	file(WRITE "${installLoc}/dev_env.json"
"{
	\"sourcePath\": \"${CMAKE_SOURCE_DIR}\",
	\"buildPath\": \"${CMAKE_BINARY_DIR}\"
}"
	)
endif()
