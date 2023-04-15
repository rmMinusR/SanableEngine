[![MSBuild](https://github.com/rmMinusR/GPR-460-01-FA2022/actions/workflows/msbuild.yml/badge.svg)](https://github.com/rmMinusR/GPR-460-01-FA2022/actions/workflows/msbuild.yml) [![EmBuild](https://github.com/rmMinusR/GPR-460-01-FA2022/actions/workflows/embuild.yml/badge.svg)](https://github.com/rmMinusR/GPR-460-01-FA2022/actions/workflows/embuild.yml)

# Setup

1. Clone this repo with `--recursive` (uses submodules for emsdk)
2. Run `emsetup.bat` to install Emscripten toolchain
3. Use pip to install `libclang`

# Building

**Windows:** Build from Visual Studio

**Emscripten:** Run `christensen-GPR460/embuild.bat`

Alternatively, use the configured GitHub Actions.