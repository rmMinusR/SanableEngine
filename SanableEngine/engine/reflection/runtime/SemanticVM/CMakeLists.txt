cmake_minimum_required (VERSION 3.14)
set (CMAKE_CXX_STANDARD 17)

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_rtti_semanticvm_sources)
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src/platform/${TARGET_ARCH_GROUP}" engine_rtti_semanticvm_platform_sources)
target_sources("engine-rtti" PRIVATE ${engine_rtti_semanticvm_sources} ${engine_rtti_semanticvm_platform_sources})
target_include_directories("engine-rtti" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")
target_include_directories("engine-rtti" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")

# Declare imports
target_link_libraries("engine-rtti" PRIVATE capstone)
