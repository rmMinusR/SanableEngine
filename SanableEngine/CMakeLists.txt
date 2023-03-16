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
include("CMakeHelperFuncs.cmake")

# Configure SDL2
if (WIN32)
    message(" > Configuring SDL2 for Win32")
    set(SDL2_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/SDL2")
    set(SDL2_DIR "${SDL2_BASE_DIR}/cmake")
elseif(EMSCRIPTEN)
    message(" > Configuring SDL2 for Emscripten")
    add_compile_options("-sUSE_SDL=2")
    add_link_options("-sUSE_SDL=2")
else()
    message(ERROR "-> Could not configure SDL2: Unknown platform")
endif()
find_package(SDL2 REQUIRED)

# Configure SDL2_image
if (MSVC)
    message(" > Configuring SDL2_image for MSVC")
    set(SDL2_image_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/SDL2_image-VC")
    set(SDL2_image_DIR "${SDL2_image_BASE_DIR}/cmake")
elseif(MINGW)
    message(" > Configuring SDL2_image for MinGW")
    set(SDL2_image_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/SDL2_image-MinGW")
    set(SDL2_image_DIR "${SDL2_image_BASE_DIR}/cmake")
elseif(EMSCRIPTEN)
    message(" > Configuring SDL2 for Emscripten")
    # No further config needed
else()
    message(ERROR "-> Could not configure SDL2_image: Unknown platform")
endif()
find_package(SDL2_image REQUIRED)

include("engine/CMakeLists.txt") # Shared common code
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
