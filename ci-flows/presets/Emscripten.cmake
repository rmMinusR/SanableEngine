cmake_minimum_required(VERSION 3.28)

set(CI_CONFIG_OS Web)
set(CI_CONFIG_RUNNER windows-latest)
set(CI_CONFIG_CMAKE_GENERATOR Ninja)
set(CI_CONFIG_CMAKE_CONFIGURE_PREFIX emcmake)

set(CI_CONFIG_ARCHITECTURE_NAME WASM) # TODO: Currently unsupported out-of-the-box by GH runner, may have to build our own Docker image


set(CI_CONFIG_COMPILER_NAME Emscripten)
set(CI_CONFIG_COMPILER_C )
set(CI_CONFIG_COMPILER_CXX )
set(CI_CONFIG_COMPILER_SETUP_ACTION "uses: mymindstorm/setup-emsdk@v11")
emit_config()
