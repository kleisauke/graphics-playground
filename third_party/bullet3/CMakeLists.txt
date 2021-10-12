FetchContent_Declare(bullet3
    GIT_REPOSITORY https://github.com/bulletphysics/bullet3
    GIT_TAG ce26271
    #[[GIT_TAG 3.1.7]])

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "Use MSVC Runtime Library DLL (/MD or /MDd)" FORCE)
endif ()

set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "Disable bullet graphical benchmarks" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "Disable Bullet 3 OpenGL3+ demos" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "Disable Bullet 2 demos" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "Disable extras" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "Disable unit tests" FORCE)

FetchContent_MakeAvailable(bullet3)
