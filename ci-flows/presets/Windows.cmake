cmake_minimum_required(VERSION 3.28)

set(CI_CONFIG_OS Windows)
set(CI_CONFIG_RUNNER windows-latest)
set(CI_CONFIG_CMAKE_GENERATOR Ninja)
set(CI_CONFIG_CMAKE_CONFIGURE_PREFIX )

set(CI_CONFIG_ARCHITECTURE_NAME x86_64) # TODO: Currently unsupported out-of-the-box by GH runner, may have to build our own Docker image


set(CI_CONFIG_COMPILER_NAME Clang)
set(CI_CONFIG_COMPILER_C clang)
set(CI_CONFIG_COMPILER_CXX clang++)
set(CI_CONFIG_COMPILER_SETUP_ACTION "run: echo dummy")
emit_config()


set(CI_CONFIG_COMPILER_NAME MSVC)
set(CI_CONFIG_COMPILER_C cl)
set(CI_CONFIG_COMPILER_CXX cl)
set(CI_CONFIG_COMPILER_SETUP_ACTION "uses: ilammy/msvc-dev-cmd@v1")
emit_config()
