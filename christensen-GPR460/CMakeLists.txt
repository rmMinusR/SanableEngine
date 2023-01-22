cmake_minimum_required (VERSION 3.8)

project ("devenv") #Ignore

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
  set(ARCH_NAME x64)
else ()
  set(ARCH_NAME x86)
endif ()

function(export_dll dll project)
	if (WIN32)
        add_custom_command(
            TARGET ${project} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${dll}>" "$<TARGET_FILE_DIR:${project}>"
            VERBATIM
        )
    endif()
endfunction()

include("engine/CMakeLists.txt")
include("PrimitivesPlugin/CMakeLists.txt")
include("UserPlugin/CMakeLists.txt")
