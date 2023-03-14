cmake_minimum_required (VERSION 3.13)

project ("devenv") #Ignore

# Color output (not supported by MSVC)
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(-fdiagnostics-color=always)
    add_link_options(-fdiagnostics-color=always)
endif()

if (EMSCRIPTEN)
    add_compile_options("-fPIC") # I don't know why this works, but without it spews errors. Maybe related to linker?
endif()

# Include function library
include("CMakeHelperFuncs.txt")

# Configure SDL2
if (WIN32)
    message(" > Configuring SDL2 for Win32")
    set(SDL2_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/SDL2")
    set(SDL2_DIR "${SDL2_BASE_DIR}/cmake")
elseif(EMSCRIPTEN)
    message(" > Configuring SDL2 for Emscripten")
    add_compile_options("-sUSE_SDL=2")
    add_link_options("-sUSE_SDL=2")
else()
    message(ERROR "-> Could not configure SDL2: Unknown platform")
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
