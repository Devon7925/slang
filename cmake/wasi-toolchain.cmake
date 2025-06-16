# Toolchain file for building Slang's slangd.wasm as a standalone WASI module (preview1)

# Treat WASI as the target system
set(CMAKE_SYSTEM_NAME WASI)
set(CMAKE_SYSTEM_VERSION 1)

# Use clang/clang++ targeting wasm32-wasi
find_program(CLANG_PATH clang)
find_program(CLANGXX_PATH clang++)
if(NOT CLANG_PATH OR NOT CLANGXX_PATH)
  message(FATAL_ERROR "clang/clang++ not found for WASI toolchain")
endif()
set(CMAKE_C_COMPILER "${CLANG_PATH}")
set(CMAKE_CXX_COMPILER "${CLANGXX_PATH}")

# Configure search paths for target system
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Default flags for WASI build
set(CMAKE_C_FLAGS_INIT       "-target wasm32-wasi -O3")
set(CMAKE_CXX_FLAGS_INIT     "-target wasm32-wasi -O3")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--no-entry -Wl,--export-all -Wl,--allow-undefined")