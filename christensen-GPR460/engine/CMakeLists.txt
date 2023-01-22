cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("engine")

message("-- Configuring SDL2")
set(SDL2_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../SDL2")
set(SDL2_DIR "${SDL2_BASE_DIR}/cmake")
find_package(SDL2 REQUIRED)

function(export_dll dll project)
	if (WIN32)
        add_custom_command(
            TARGET ${project} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${dll}>" "$<TARGET_FILE_DIR:${project}>"
            VERBATIM
        )
    endif()
endfunction()

include("${CMAKE_CURRENT_LIST_DIR}/core/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/vendor/CMakeLists.txt")
