cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("engine-vendor")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_vendor_sources)
add_executable("engine-vendor" ${engine_vendor_sources})
target_include_directories("engine-vendor" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare post-build action: Copy SDL2.dll to output dir
if (WIN32)
    add_custom_command(
        TARGET engine-vendor POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:engine-vendor>"
        VERBATIM
    )
endif()

# Declare imports
target_link_libraries("engine-vendor" "engine-core")