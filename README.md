# Sanable Engine & STIX

[![MSBuild](https://github.com/rmMinusR/GPR-460-01-FA2022/actions/workflows/msbuild.yml/badge.svg)](https://github.com/rmMinusR/GPR-460-01-FA2022/actions/workflows/msbuild.yml) [![EmBuild](https://github.com/rmMinusR/GPR-460-01-FA2022/actions/workflows/embuild.yml/badge.svg)](https://github.com/rmMinusR/GPR-460-01-FA2022/actions/workflows/embuild.yml)

**Sanable Engine** is a game framework focusing on hot-reloadable plugins for rapid iteration, building to desktop and web. Many of its more advanced features are made possible by STIX, such as hot reloading, type-aware memory pooling, and arbitrary message dispatch.

Sanable Type Info eXtensions (**STIX**) is a C++ runtime reflection library originally developed for Sanable, now available for use in any project. It is compiler and platform agnostic, non-intrusive, and standard-compliant, while offering full introspection including private member access, vtable pointer and padding detection, and complete function pointer erasure with just one call.

# STIX Philosophy

**Unintrusive**: If it can build without STIX, it should be able to build with STIX. End-users should be able to write code however they want, with minimal restrictions on file format, naming, or language features. No `BEGIN_CLASS` or `GENERATED_BODY()`, no `friend` declarations, no maintaining field lists by hand.

**Configurable**: End-users should be able to control how much introspection STIX does, globally for a translation unit or on a per-symbol basis, and control the means of that introspection.

**Portable**: STIX should always have a base configuration that is compiler, OS, and architecture agnostic, relying only on standard features and the occasional well-defined but unspoken pattern. However it may offer more specific, non-portable options (such as extracting vtables by type-2 virtualization, which is currently only available on x86-64).

# Getting Started

1. `git clone https://github.com/rmMinusR/SanableEngine.git --recursive`
2. Install Python 3
3. Use pip to install `libclang`
4. (Optional) If building for web, install the Emscripten toolchain. An example is provided in `emsetup.bat`.

## Building

### Build from IDE

Sanable/STIX are CMake projects. Most IDEs can build using CMake, but some (like Visual Studio) may require you to install an additional package.

### Build from command-line

**Windows:**
- `cmake -S SanableEngine/ -B build`
- `cmake build`
- (Optional) `cmake --install build --prefix install`

**Web:** Run `embuild.bat`, or:
- Run the appropriate `emsdk_env`
- `encmake cmake -S SanableEngine/ -B build`
- `cmake build`
- (Optional) `cmake --install build --prefix install`
- Run `python -m http.server` from within your build/install folder, and open a web browser to <localhost:8000>.
