cmake_minimum_required (VERSION 3.13)

project ("devenv") #Ignore

# Color output (not supported by MSVC)
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(-fdiagnostics-color=always)
    add_link_options(-fdiagnostics-color=always)
endif()

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

function(export_plugin_dll project)
	if (WIN32)
        set_target_properties(${project} PROPERTIES
	        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/${project}"
	        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/${project}"
	        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/${project}"
        )
    endif()
endfunction()

# Configure SDL2
message("-- Configuring SDL2")
if (WIN32)
    set(SDL2_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/SDL2")
    set(SDL2_DIR "${SDL2_BASE_DIR}/cmake")
endif()
if (EMSCRIPTEN) # Support Emscripten
    message("-- Detected Emscripten")
    #set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    #set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sUSE_SDL=2")
    #set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -sUSE_SDL=2")
    add_link_options("-sUSE_SDL=2")
endif()
find_package(SDL2 REQUIRED)

include("engine/core/CMakeLists.txt") # Shared common code
include("PrimitivesPlugin/CMakeLists.txt")
include("UserPlugin/CMakeLists.txt")

# Platform-specific builds
# Must be done last so Emscripten can pack plugins as assets in VFS
if (EMSCRIPTEN)
    include("engine/platform/emscripten/CMakeLists.txt")
endif()
if (WIN32)
    include("engine/platform/win32/CMakeLists.txt")
endif()
