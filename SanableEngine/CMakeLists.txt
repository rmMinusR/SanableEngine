cmake_minimum_required (VERSION 3.13)

project ("devenv") #Ignore
enable_testing()

include("Utils.cmake") # Include function library
include("libs/Imports.cmake") # Include libs folder

add_compile_definitions(NOMINMAX) # Come on, MSVC. Don't conflict with standard libraries.

include("engine/CMakeLists.txt") # Shared common code

# YOUR PLUGINS HERE
include("PluginControlUI/CMakeLists.txt")
include("PrimitivesPlugin/CMakeLists.txt")
include("UserPlugin/CMakeLists.txt")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/LICENSE.txt" DESTINATION "./licenses" RENAME "Sanable.txt")
package_assets() # Must be done last so Emscripten knows how to build VFS
