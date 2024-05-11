cmake_minimum_required(VERSION 3.28)

project(Sanable-GitHub-CI)

set(CI_OUTPUT_DIR "${PROJECT_BINARY_DIR}/flows/")
message("Will build to ${CI_OUTPUT_DIR}, install to ${CMAKE_INSTALL_PREFIX}")
set(CI_OUTPUT_PREFIX "build-")
#file(GLOB DIRTY "${CI_OUTPUT_DIR}/${CI_OUTPUT_PREFIX}*.yml") # TODO
file(REMOVE ${CI_OUTPUT_DIR})
function(emit_config)
    # Expects the following variables:
    # CI_CONFIG_OS
    # CI_CONFIG_RUNNER
    # CI_CONFIG_CMAKE_CONFIGURE_PREFIX
    # CI_CONFIG_CMAKE_GENERATOR
    # CI_CONFIG_COMPILER_NAME
    # CI_CONFIG_COMPILER_C
    # CI_CONFIG_COMPILER_CXX
    # CI_CONFIG_COMPILER_SETUP_ACTION
    # CI_CONFIG_ARCHITECTURE_NAME - Currently unsupported out-of-the-box by GH runner, may have to build our own Docker image
    set(CI_CONFIG_DISPLAYNAME "${CI_CONFIG_OS}-${CI_CONFIG_COMPILER_NAME}-${CI_CONFIG_ARCHITECTURE_NAME}")
    configure_file("${PROJECT_SOURCE_DIR}/build-and-test-template.yml" "${CI_OUTPUT_DIR}/${CI_OUTPUT_PREFIX}${CI_CONFIG_DISPLAYNAME}.yml" @ONLY)
    install(FILES "${CI_OUTPUT_DIR}/${CI_OUTPUT_PREFIX}${CI_CONFIG_DISPLAYNAME}.yml" DESTINATION ${CMAKE_INSTALL_PREFIX})
endfunction()

include(presets/Windows.cmake)
include(presets/Emscripten.cmake)
