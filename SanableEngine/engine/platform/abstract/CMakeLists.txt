cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

##############################################################################################
##																					        ##
##  "Abstract" is not a platform, but an interface. All platforms must fully implement it.  ##
##																					        ##
##############################################################################################

project("engine-platform-abstract")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_platform_abstract_sources)
add_library("engine-platform-abstract" STATIC ${engine_platform_abstract_sources})
target_include_directories("engine-platform-abstract" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")
target_include_directories("engine-platform-abstract" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
target_link_libraries("engine-platform-abstract" PRIVATE glew)
