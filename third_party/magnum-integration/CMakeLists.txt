FetchContent_Declare(magnum-integration
    GIT_REPOSITORY https://github.com/mosra/magnum-integration.git
    GIT_TAG 8fdf147
    #[[GIT_TAG v2020.06]]
    GIT_SUBMODULES "")

set(MAGNUM_WITH_BULLET ON CACHE BOOL "Build BulletIntegration library" FORCE)
set(MAGNUM_WITH_IMGUI ON CACHE BOOL "Build ImGuiIntegration library" FORCE)

FetchContent_MakeAvailable(magnum-integration)

# Disable pedantic compiler flags in 3rd party code when not compiling with MSVC
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(DIRECTORY "${magnum-integration_SOURCE_DIR}/src" PROPERTY CORRADE_USE_PEDANTIC_FLAGS OFF)
endif ()

set(CMAKE_MODULE_PATH "${magnum-integration_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH} PARENT_SCOPE)
