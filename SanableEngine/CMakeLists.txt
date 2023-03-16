cmake_minimum_required (VERSION 3.13)

project ("devenv") #Ignore

if (EMSCRIPTEN)
    add_compile_options("-fPIC") # I don't know why this works, but without it spews errors. Maybe related to linker?
endif()

include("Utils.cmake") # Include function library
include("Imports.cmake") # Include libs folder

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
