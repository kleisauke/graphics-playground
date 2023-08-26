find_package(Magnum REQUIRED
    GL
    MeshTools
    Primitives
    SceneGraph
    Shaders
    Trade)
find_package(MagnumIntegration REQUIRED
    Bullet
    ImGui)
find_package(Bullet REQUIRED Dynamics)

if (EMSCRIPTEN)
    find_package(Magnum REQUIRED EmscriptenApplication)
else ()
    find_package(Magnum REQUIRED Sdl2Application)
endif ()

# Only enable pedantic compiler flags when compiling with MSVC
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)
endif ()

add_executable(playground WIN32
    Application.cpp
    ColoredDrawable.cpp
    ColoredDrawable.h
    GravityBox.cpp
    GravityBox.h
    InstanceData.h
    MovingSphere.cpp
    MovingSphere.h
    OrbitCamera.cpp
    OrbitCamera.h
    Rigidbody.cpp
    Rigidbody.h)
target_link_libraries(playground PRIVATE
    Corrade::Main
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::MeshTools
    Magnum::Primitives
    Magnum::SceneGraph
    Magnum::Shaders
    Magnum::Trade
    MagnumIntegration::Bullet
    Bullet::Dynamics
    MagnumIntegration::ImGui)
