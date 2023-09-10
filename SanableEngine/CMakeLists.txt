cmake_minimum_required (VERSION 3.13)

project ("devenv") #Ignore

include("Utils.cmake") # Include function library
include("Imports.cmake") # Include libs folder

add_compile_definitions(NOMINMAX) # Come on, MSVC. Don't conflict with standard libraries.

include("engine/CMakeLists.txt") # Shared common code

# YOUR PLUGINS HERE
include("PluginControlUI/CMakeLists.txt")
include("PrimitivesPlugin/CMakeLists.txt")
include("UserPlugin/CMakeLists.txt")

package_assets() # Must be done last so Emscripten knows how to build VFS
