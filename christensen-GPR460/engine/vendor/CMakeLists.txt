cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("engine-vendor")

# Declare target
aux_source_directory("vendor/src" engine_vendor_sources)
add_executable("engine-vendor" ${engine_vendor_sources})
target_include_directories("engine-vendor" PRIVATE "vendor/private")

# Declare imports
target_link_libraries("engine-vendor" "engine-core")
