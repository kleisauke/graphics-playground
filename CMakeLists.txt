# Note: FetchContent_MakeAvailable requires CMake 3.14
cmake_minimum_required(VERSION 3.14)

project(graphics-playground
    VERSION 0.0.1
    DESCRIPTION "Graphics playground"
    LANGUAGES CXX)

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
