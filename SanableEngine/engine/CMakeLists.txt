cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

project("engine-shared")

include ("${CMAKE_CURRENT_LIST_DIR}/reflection/CMakeLists.txt")
include ("${CMAKE_CURRENT_LIST_DIR}/memory/CMakeLists.txt")
include ("${CMAKE_CURRENT_LIST_DIR}/platform/abstract/CMakeLists.txt")
include ("${CMAKE_CURRENT_LIST_DIR}/assets/CMakeLists.txt")
include ("${CMAKE_CURRENT_LIST_DIR}/core/CMakeLists.txt")
include ("${CMAKE_CURRENT_LIST_DIR}/graphics/CMakeLists.txt")

# Platform-specific builds
if (EMSCRIPTEN)
    include("engine/platform/emscripten/CMakeLists.txt")
elseif (WIN32)
    include("engine/platform/win32/CMakeLists.txt")
else()
    message(ERROR " > Unknown platform")
endif()
