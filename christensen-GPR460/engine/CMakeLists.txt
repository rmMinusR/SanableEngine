cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("engine")

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
  set(ARCH_NAME x64)
else ()
  set(ARCH_NAME x86)
endif ()

message("-- Configuring SDL2")
set(SDL2_BASE_DIR "${PROJECT_SOURCE_DIR}/../SDL2")
set(SDL2_DIR "${SDL2_BASE_DIR}/cmake")
find_package(SDL2 REQUIRED)

include("core/CMakeLists.txt")
include("vendor/CMakeLists.txt")
