cmake_minimum_required (VERSION 3.8)

project ("devenv") #Ignore

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
  set(ARCH_NAME x64)
else ()
  set(ARCH_NAME x86)
endif ()

include("engine/CMakeLists.txt")
include("PrimitivesPlugin/CMakeLists.txt")
