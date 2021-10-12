FetchContent_Declare(magnum
    GIT_REPOSITORY https://github.com/mosra/magnum.git
    GIT_TAG d96957b
    #[[GIT_TAG v2020.06]]
    GIT_SUBMODULES ""
    PATCH_COMMAND git apply "${CMAKE_CURRENT_SOURCE_DIR}/magnum.patch")

if (EMSCRIPTEN)
    set(WITH_EMSCRIPTENAPPLICATION ON CACHE BOOL "Build EmscriptenApplication library" FORCE)
else ()
    set(WITH_SDL2APPLICATION ON CACHE BOOL "Build Sdl2Application library" FORCE)
endif ()

set(BUILD_DEPRECATED OFF CACHE BOOL "Exclude deprecated API in the build" FORCE)

FetchContent_MakeAvailable(magnum)

# Disable pedantic compiler flags in 3rd party code when not compiling with MSVC
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(DIRECTORY "${magnum_SOURCE_DIR}/src" PROPERTY CORRADE_USE_PEDANTIC_FLAGS OFF)
endif ()

set(CMAKE_MODULE_PATH "${magnum_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH} PARENT_SCOPE)
