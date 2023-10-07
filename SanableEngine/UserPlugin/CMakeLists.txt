cmake_minimum_required (VERSION 3.8)

project ("UserGameplayPlugin")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" UserGameplayPlugin_sources)
declare_plugin("UserGameplayPlugin" UserGameplayPlugin_sources)
target_include_directories("UserGameplayPlugin" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_link_libraries("UserGameplayPlugin" "PrimitivesPlugin")
target_link_libraries("UserGameplayPlugin" "engine-core")

# Declare exports
target_include_directories("UserGameplayPlugin" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")

add_custom_command(
    TARGET UserGameplayPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_LIST_DIR}/resources/bunny.fbx
            ${CMAKE_CURRENT_BINARY_DIR}/resources/bunny.fbx)
