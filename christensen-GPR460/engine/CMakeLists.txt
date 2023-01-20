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
set( SDL_STATIC ON CACHE BOOL "" FORCE )
set( SDL_SHARED OFF CACHE BOOL "" FORCE )
set(SDL2_BASE_DIR "${PROJECT_SOURCE_DIR}/../SDL2")
set(SDL2_DIR "${SDL2_BASE_DIR}/cmake")
find_package(SDL2 REQUIRED)

# Support both 32 and 64 bit builds
set(SDL2_LIBRARIES "${SDL2_BASE_DIR}/lib/${ARCH_NAME}/SDL2.lib;${SDL2_BASE_DIR}/lib/${ARCH_NAME}/SDL2main.lib")
string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

message("SDL includes: ${SDL2_INCLUDE_DIRS}")
message("SDL libs: ${SDL2_LIBRARIES}")

include("core/CMakeLists.txt")
include("vendor/CMakeLists.txt")
