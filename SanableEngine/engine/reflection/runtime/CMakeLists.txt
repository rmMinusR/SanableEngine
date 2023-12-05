cmake_minimum_required (VERSION 3.14)
set (CMAKE_CXX_STANDARD 17)

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_rtti_sources)
declare_dll("engine-rtti" "" engine_rtti_sources)
#add_library("engine-rtti" STATIC ${engine_rtti_sources})
target_include_directories("engine-rtti" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare exports
target_include_directories("engine-rtti" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")

include("${CMAKE_CURRENT_LIST_DIR}/SemanticVM/CMakeLists.txt")

# Declare install targets
install_dll("engine-rtti" ".")
