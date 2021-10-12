# Note: FetchContent_MakeAvailable requires CMake 3.14
cmake_minimum_required(VERSION 3.14)

project(graphics-playground
    VERSION 0.0.1
    DESCRIPTION "Graphics playground"
    LANGUAGES CXX
    )

macro(append_compiler_flags FLAGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")
endmacro()

macro(append_compiler_flags_debug FLAGS)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${FLAGS}")
endmacro()

macro(append_linker_flags FLAGS)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FLAGS}")
endmacro()

macro(append_linker_flags_debug FLAGS)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${FLAGS}")
endmacro()

macro(append_linker_flags_opts FLAGS)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} ${FLAGS}")
endmacro()

macro(append_linker_flags_minsizerel FLAGS)
    set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} ${FLAGS}")
endmacro()

# Emscripten-specific options
if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    # Handy for debugging
    append_compiler_flags_debug("-gsource-map")
    append_linker_flags_debug("-gsource-map -sASSERTIONS=1 --source-map-base http://localhost:8080/")

    # Common compiler flags
    append_compiler_flags("-fno-rtti -fno-exceptions -mnontrapping-fptoint -DCORRADE_NO_ASSERT")

    # Enable fixed-width SIMD by default
    append_compiler_flags("-msimd128")

    # Enable JS BigInt to Wasm i64 integration by default
    append_linker_flags("-sWASM_BIGINT")

    # Common linker flags
    append_linker_flags("-sTEXTDECODER=2 -sABORTING_MALLOC=0 -sSUPPORT_ERRNO=0")
    append_linker_flags("-sENVIRONMENT=web -sMIN_FIREFOX_VERSION=89 -sMIN_EDGE_VERSION=91 -sMIN_CHROME_VERSION=91")
    append_linker_flags("--shell-file \"${CMAKE_SOURCE_DIR}/build/shell.html\"")
    append_linker_flags("-sALLOW_MEMORY_GROWTH=1")
    append_linker_flags("-sFILESYSTEM=0 -sAUTO_JS_LIBRARIES=0 -sDISABLE_EXCEPTION_THROWING=1")
    append_linker_flags("-lhtml5.js -lwebgl.js -lwebgl2.js")
    append_linker_flags("-sFULL_ES3=1 -sMAX_WEBGL_VERSION=2")

    append_linker_flags("-sINITIAL_MEMORY=16MB -sMALLOC=emmalloc")

    append_linker_flags_minsizerel("-Oz")

    append_linker_flags_opts("-sASSERTIONS=0 --closure 1")
endif ()

include(FetchContent)

# Enable verbose FetchContent output
set(FETCHCONTENT_QUIET OFF)

# Bullet Physics SDK
# https://github.com/bulletphysics/bullet3
add_subdirectory(third_party/bullet3)

# Dear ImGui
# https://github.com/ocornut/imgui
add_subdirectory(third_party/imgui)

# Corrade
# https://github.com/mosra/corrade
add_subdirectory(third_party/corrade)

# Magnum
# https://github.com/mosra/magnum
add_subdirectory(third_party/magnum)

# Magnum integration libraries
# https://github.com/mosra/magnum-integration
add_subdirectory(third_party/magnum-integration)

add_subdirectory(src)
