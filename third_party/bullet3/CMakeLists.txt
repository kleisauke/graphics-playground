FetchContent_Declare(bullet3
    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
    #[[GIT_TAG 7dee343]]
    GIT_TAG 3.25)

# Silence CMake 3.28.x deprecation warning
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Minimum policy version for CMake" FORCE)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "Use MSVC Runtime Library DLL (/MD or /MDd)" FORCE)
endif ()

set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "Disable bullet graphical benchmarks" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "Disable Bullet 3 OpenGL3+ demos" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "Disable Bullet 2 demos" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "Disable extras" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "Disable unit tests" FORCE)

FetchContent_MakeAvailable(bullet3)
