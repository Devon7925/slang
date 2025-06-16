#
# WebAssembly bindings for Slang
#
# This is an executable target because emcmake produces .a files without bindings if you just create a static library
# https://stackoverflow.com/questions/63622009/static-library-built-with-cmake-as-a-with-emscripten-instead-of-wasm-js
### WebAssembly builds: Emscripten (for slang-wasm) or WASI (for slangd) ###
if(EMSCRIPTEN OR CMAKE_SYSTEM_NAME STREQUAL "WASI")
    # slang-wasm via Emscripten only
    if(EMSCRIPTEN)
        slang_add_target(
            .
            EXECUTABLE
            EXCLUDE_FROM_ALL
            USE_FEWER_WARNINGS
            LINK_WITH_PRIVATE
                miniz
                lz4_static
                slang
                core
                compiler-core
                slang-capability-defs
                slang-capability-lookup
                slang-fiddle-output
                slang-lookup-tables
            INCLUDE_DIRECTORIES_PUBLIC ${slang_SOURCE_DIR}/include .
        )
        target_link_options(
            slang-wasm
            PUBLIC "--bind" "--emit-tsd" "interface.d.ts" "-s" "EXPORT_ES6=1"
        )
    endif()

    # slangd: Emscripten or standalone WASI module
    slang_add_target(
        ${slang_SOURCE_DIR}/tools/slangd
        EXECUTABLE
        EXCLUDE_FROM_ALL
        USE_FEWER_WARNINGS
        LINK_WITH_PRIVATE
            miniz
            lz4_static
            core
            compiler-core
            slang
            slang-capability-defs
            slang-fiddle-output
            slang-lookup-tables
        INCLUDE_DIRECTORIES_PUBLIC ${slang_SOURCE_DIR}/include ${slang_SOURCE_DIR}/tools/slangd
    )
    if(EMSCRIPTEN)
        target_link_options(
            slangd
            PUBLIC "--bind" "--emit-tsd" "slangd.d.ts" "-s" "EXPORT_ES6=1"
        )
    elseif(CMAKE_SYSTEM_NAME STREQUAL "WASI")
        target_link_options(
            slangd
            PUBLIC "-Wl,--no-entry" "-Wl,--export-all" "-Wl,--allow-undefined"
        )
    endif()
endif()
