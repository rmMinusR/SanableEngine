cmake_minimum_required (VERSION 3.13)

project ("devenv") #Ignore

if (EMSCRIPTEN)
    add_compile_options("-fPIC") # Emscripten side modules require PIC. Might as well turn it on globally.
endif()

# Include function library
include("CMakeHelperFuncs.cmake")

# Set debug flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    message(" > Detected debug build. Names will be left in.")
    #add_compile_options(-g3)
    #add_link_options(-g3)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    message(" > Detected release build. Output will be optimized.")
    add_compile_options(-O2)
    add_link_options(-O2)
else()
    message(ERROR " > Unknown build type ${CMAKE_BUILD_TYPE}")
endif()

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

include("engine/CMakeLists.txt") # Shared common code

# YOUR PLUGINS HERE
include("PrimitivesPlugin/CMakeLists.txt")
include("UserPlugin/CMakeLists.txt")

package_assets() # Must be done last so Emscripten can build VFS