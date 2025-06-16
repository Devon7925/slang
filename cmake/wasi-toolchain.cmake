# Toolchain file for building Slang's slangd.wasm as a standalone WASI module (preview1)

# Require a WASI sysroot from WASI_SDK_PATH
if(NOT DEFINED ENV{WASI_SDK_PATH})
  message(FATAL_ERROR "WASI_SDK_PATH environment variable must point to a wasi-sdk installation")
endif()
set(WASI_SDK_PATH $ENV{WASI_SDK_PATH})

# Treat WASI as the target system and use the SDK's clang wrappers
set(CMAKE_SYSTEM_NAME WASI)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_C_COMPILER   "${WASI_SDK_PATH}/bin/clang")
set(CMAKE_CXX_COMPILER "${WASI_SDK_PATH}/bin/clang++")

# Configure search paths for the WASI sysroot
set(CMAKE_SYSROOT "${WASI_SDK_PATH}/share/wasi-sysroot")
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# The clang wrapper already configures flags and sysroot