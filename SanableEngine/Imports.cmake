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
    find_package(SDL2_image REQUIRED)
elseif(MINGW)
    message(" > Configuring SDL2_image for MinGW")
    set(SDL2_image_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/SDL2_image-MinGW")
    set(SDL2_image_DIR "${SDL2_image_BASE_DIR}/cmake")
    find_package(SDL2_image REQUIRED)
elseif(EMSCRIPTEN)
    message(" > Configuring SDL2_image for Emscripten")
    add_compile_options("-sUSE_SDL_IMAGE=2 --use-preload-plugins")
    add_link_options("-sUSE_SDL_IMAGE=2 --use-preload-plugins")
else()
    message(ERROR "-> Could not configure SDL2_image: Unknown platform")
endif()

# Configure stb_image
message(" > Configuring stb_image")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/stb_image")

# Configure SDL2_ttf
if(FALSE) # Disabled: Using Freetype instead
    if (MSVC)
        message(" > Configuring SDL2_ttf for MSVC")
        set(SDL2_ttf_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/SDL2_ttf-VC")
        set(SDL2_ttf_DIR "${SDL2_ttf_BASE_DIR}/cmake")
    elseif(MINGW)
        message(" > Configuring SDL2_ttf for MinGW")
        set(SDL2_ttf_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/SDL2_ttf-MinGW")
        set(SDL2_ttf_DIR "${SDL2_ttf_BASE_DIR}/cmake")
    elseif(EMSCRIPTEN)
        message(" > Configuring SDL2_ttf for Emscripten")
        add_compile_options("-sUSE_SDL_TTF=2 --use-preload-plugins")
        add_link_options("-sUSE_SDL_TTF=2 --use-preload-plugins")
    else()
        message(ERROR "-> Could not configure SDL2_ttf: Unknown platform")
    endif()
    find_package(SDL2_ttf REQUIRED)
endif()

# Configure Freetype
if(EMSCRIPTEN)
    message(" > Configuring Freetype for Emscripten")
    add_compile_options("-sUSE_FREETYPE=1")
    add_link_options("-sUSE_FREETYPE=1")
else(WIN32)
    message(" > Configuring Freetype for desktop")
    set(FREETYPE_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/freetype/")
    #add_subdirectory("${FREETYPE_BASE_DIR}")
endif()
find_package(Freetype REQUIRED)

# Configure GLM
message(" > Configuring GLM")
set(glm_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/glm")
set(glm_DIR "${glm_BASE_DIR}/cmake/glm")
find_package(glm REQUIRED)

# Configure OpenFBX
message(" > Configuring OpenFBX")
set(OpenFBX_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/OpenFBX")
# This version of OpenFBX has a CMakeLists that incorrectly depends on miniz. Add target manually.
add_library(OpenFBX STATIC "${OpenFBX_BASE_DIR}/src/ofbx.cpp" "${OpenFBX_BASE_DIR}/src/libdeflate.c")
target_include_directories(OpenFBX PUBLIC "${OpenFBX_BASE_DIR}/src/")

# Configure GLEW
if(WIN32)
    message(" > Configuring GLEW for Win32")
    set(GLEW_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/glew")

    if(NOT EXISTS "${GLEW_DIR}/src/glew.c")
        message(FATAL_ERROR "Please generate GLEW sources before building: Go to glew/auto and run make.")
    endif()

    # This version of GLEW has a broken configfile. Add targets manually.
    add_library(glew STATIC "${GLEW_DIR}/src/glew.c") # NOTE: If this errors, 
    target_compile_definitions(glew PUBLIC GLEW_STATIC)
    target_compile_definitions(glew PRIVATE GLEW_BUILD)
    target_include_directories(glew PUBLIC "${GLEW_DIR}/include/")
elseif(EMSCRIPTEN)
    message(" > Configuring GLEW for Emscripten")
    add_compile_options("-sUSE_GLEW=2 --use-preload-plugins")
    add_link_options("-sUSE_GLEW=2 --use-preload-plugins")
else()
    message(ERROR "-> Could not configure GLEW: Unknown platform")
endif()

# Delegate doctest configuration: https://stackoverflow.com/a/72984086
 include(FetchContent)
 FetchContent_Declare(
     doctest
     GIT_REPOSITORY "https://github.com/doctest/doctest"
 )
 FetchContent_MakeAvailable(doctest)
include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")

# Configure Capstone disassembly engine
set(SANABLE_DISASSEMBLER "Capstone")
if (SANABLE_DISASSEMBLER STREQUAL "Capstone")
    message(" > Configuring Capstone disassembler for ${CMAKE_SYSTEM_PROCESSOR}")
    #set(CAPSTONE_BUILD_DIET ON) # Don't disable, we need this in X86 mode

    # Disable all architectures
    #set(CAPSTONE_ARCHITECTURE_DEFAULT OFF) # Broken
    set(SUPPORTED_ARCHITECTURES ARM ARM64 M68K MIPS PPC SPARC SYSZ XCORE X86 TMS320C64X M680X EVM MOS65XX WASM BPF RISCV SH TRICORE)
    foreach(supported_architecture ${SUPPORTED_ARCHITECTURES})
        set("CAPSTONE_${supported_architecture}_SUPPORT" OFF)
    endforeach()

    # Detect our architecture
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set (TARGET_ARCH_GROUP X86)
    else()
        set (TARGET_ARCH_GROUP ${CMAKE_SYSTEM_PROCESSOR})
    endif()

    # Enable our architecture and set flags
    if (${TARGET_ARCH_GROUP} IN_LIST SUPPORTED_ARCHITECTURES)
        set(CAPSTONE_${TARGET_ARCH_GROUP}_SUPPORT ON)
        add_definitions(-DCS_ARCH_OURS=CS_ARCH_${TARGET_ARCH_GROUP})
        message(" >> Detected family ${TARGET_ARCH_GROUP}")
    else()
        message(FATAL_ERROR "-> Could not configure Capstone: Unknown processor")
    endif()
    
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/capstone/")
else()
    message(FATAL_ERROR "-> Could not find a valid disassembler")
endif()
