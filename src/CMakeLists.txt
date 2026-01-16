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

# Emscripten-specific options
if (EMSCRIPTEN)
    set_target_properties(playground PROPERTIES SUFFIX .html)
    target_link_libraries(playground PUBLIC
        html5.js
        webgl.js
        webgl2.js)
    target_link_options(playground PUBLIC
        --shell-file=${CMAKE_CURRENT_SOURCE_DIR}/shell.html
        -sTEXTDECODER=2
        -sMALLOC=emmalloc
        -sALLOW_MEMORY_GROWTH=1
        -sENVIRONMENT=web
        -sMIN_WEBGL_VERSION=2
        -sMIN_CHROME_VERSION=91
        -sMIN_FIREFOX_VERSION=89
        -sMIN_SAFARI_VERSION=160400
        -sFILESYSTEM=0
        -sAUTO_JS_LIBRARIES=0
        -sAUTO_NATIVE_LIBRARIES=0
        -sDISABLE_EXCEPTION_THROWING=1
        $<$<CONFIG:Debug>:-gsource-map -sASSERTIONS=1>
        $<$<CONFIG:Release>:-sASSERTIONS=0 --closure 1>
        $<$<CONFIG:MinSizeRel>:-Oz -sASSERTIONS=0 --closure 1>)
    install(FILES
        $<TARGET_FILE_DIR:playground>/playground.js
        $<TARGET_FILE_DIR:playground>/playground.wasm
        $<TARGET_FILE_DIR:playground>/playground.html
        DESTINATION dist COMPONENT playground)
endif ()
