cmake_minimum_required (VERSION 3.14)
set (CMAKE_CXX_STANDARD 17)

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_rtti_sources)
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src/platform/${TARGET_ARCH_GROUP}" engine_rtti_platform_sources)
list(APPEND engine_rtti_sources ${engine_rtti_platform_sources})
declare_dll("engine-rtti" "" engine_rtti_sources)
#add_library("engine-rtti" STATIC ${engine_rtti_sources})
target_include_directories("engine-rtti" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_link_libraries("engine-rtti" PRIVATE capstone)

# Declare exports
target_include_directories("engine-rtti" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")

# Declare install targets
install_dll("engine-rtti" ".")
