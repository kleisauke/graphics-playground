#include "ColoredDrawable.h"
#include "GravityBox.h"
#include "MovingSphere.h"
#include "OrbitCamera.h"
#include "Rigidbody.h"

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Magnum/BulletIntegration/DebugDraw.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/Transform.h>
#include <Magnum/Timeline.h>
#ifdef CORRADE_TARGET_EMSCRIPTEN
#include <Magnum/Platform/EmscriptenApplication.h>
#else
#include <Magnum/Platform/Sdl2Application.h>
#endif
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/UVSphere.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/PhongGL.h>
#include <Magnum/Trade/MeshData.h>

namespace GraphicsPlayground {

using namespace Magnum;
using namespace Math::Literals;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class Application : public Platform::Application {
 public:
    explicit Application(const Arguments &arguments);

 private:
    void viewportEvent(ViewportEvent &event) override;
    void keyPressEvent(KeyEvent &event) override;
    void keyReleaseEvent(KeyEvent &event) override;
    void mousePressEvent(MouseEvent &event) override;
    void mouseReleaseEvent(MouseEvent &event) override;
    void mouseMoveEvent(MouseMoveEvent &event) override;
    void mouseScrollEvent(MouseScrollEvent &event) override;
    void textInputEvent(TextInputEvent &event) override;
    void drawEvent() override;
    void showMenu();

    ImGuiIntegration::Context _imgui{NoCreate};

    Color4 _clearColor = 0x000000ff_rgbaf;

    GL::Mesh _box{NoCreate}, _sphere{NoCreate};
    GL::Buffer _boxInstanceBuffer{NoCreate}, _sphereInstanceBuffer{NoCreate};
    Shaders::PhongGL _shader{NoCreate};
    BulletIntegration::DebugDraw _debugDraw{NoCreate};
    Containers::Array<InstanceData> _boxInstanceData, _sphereInstanceData;

    btDbvtBroadphase _bBroadphase;
    btDefaultCollisionConfiguration _bCollisionConfig;
    btCollisionDispatcher _bDispatcher{&_bCollisionConfig};
    btSequentialImpulseConstraintSolver _bSolver;

    /* The world has to live longer than the scene because RigidBody
       instances have to remove themselves from it on destruction */
    btDiscreteDynamicsWorld _bWorld{&_bDispatcher, &_bBroadphase, &_bSolver,
                                    &_bCollisionConfig};

    Scene3D _scene;
    SceneGraph::Camera3D *_camera;
    SceneGraph::DrawableGroup3D _drawables;
    Timeline _timeline;

    OrbitCamera *_orbitCamera;

    MovingSphere *_ball;
    Vector3 _playerInput;
    Vector2 _cameraInput;
    bool _desiredJump{false};

    GravityBox *_gravityBox;

    btBoxShape _bBoxShape{{0.5f, 0.5f, 0.5f}};
    btSphereShape _bSphereShape{0.5f};
    btBoxShape _bGroundShape{{4.0f, 4.0f, 4.0f}};

    bool _showMenu{false}, _drawCubes{true}, _drawDebug{true};
};

Application::Application(const Arguments &arguments)
    : Platform::Application(arguments, NoCreate) {
    /* Try 8x MSAA, fall back to zero samples if not possible. Enable only 2x
       MSAA if we have enough DPI. */
    {
        const Vector2 dpiScaling = this->dpiScaling({});
        Configuration conf;
        conf.setTitle("Graphics playground")
            .setSize(conf.size(), dpiScaling)
            .setWindowFlags(Configuration::WindowFlag::Resizable);
        GLConfiguration glConf;
        glConf.setSampleCount(dpiScaling.max() < 2.0f ? 8 : 2);
        if (!tryCreate(conf, glConf))
            create(conf, glConf.setSampleCount(0));
    }

    /* Setup ImGui */
    ImGui::CreateContext();

    /* Disable file-system access for Emscripten, this prevents opening/writing
       the imgui.ini file */
#ifdef CORRADE_TARGET_EMSCRIPTEN
    ImGui::GetIO().IniFilename = nullptr;
#endif

    /* Setup ImGui style */
    ImGui::StyleColorsDark();

    _imgui = ImGuiIntegration::Context(*ImGui::GetCurrentContext(),
                                       Vector2{windowSize()} / dpiScaling(),
                                       windowSize(), framebufferSize());

    /* Set up proper blending to be used by ImGui */
    GL::Renderer::setBlendFunction(
        GL::Renderer::BlendFunction::SourceAlpha,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    /* Camera setup */
    _orbitCamera = new OrbitCamera{&_scene};
    (_camera = new SceneGraph::Camera3D(*_orbitCamera))
        ->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.3f, 100.0f))
        .setViewport(GL::defaultFramebuffer.viewport().size());

    /* Create an instanced shader */
    _shader = Shaders::PhongGL{Shaders::PhongGL::Configuration{}.setFlags(
        Shaders::PhongGL::Flag::VertexColor |
        Shaders::PhongGL::Flag::InstancedTransformation)};
    _shader.setAmbientColor(0x111111_rgbf)
        .setSpecularColor(0x330000_rgbf)
        .setLightPositions({{10.0f, 15.0f, 5.0f, 0.0f}});

    /* Box and sphere mesh, with an (initially empty) instance buffer */
    _box = MeshTools::compile(Primitives::cubeSolid());
    _sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));
    _boxInstanceBuffer = GL::Buffer{};
    _sphereInstanceBuffer = GL::Buffer{};
    _box.addVertexBufferInstanced(
        _boxInstanceBuffer, 1, 0, Shaders::PhongGL::TransformationMatrix{},
        Shaders::PhongGL::NormalMatrix{}, Shaders::PhongGL::Color3{});
    _sphere.addVertexBufferInstanced(
        _sphereInstanceBuffer, 1, 0, Shaders::PhongGL::TransformationMatrix{},
        Shaders::PhongGL::NormalMatrix{}, Shaders::PhongGL::Color3{});

    /* Setup the renderer so we can draw the debug lines on top */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::enable(GL::Renderer::Feature::PolygonOffsetFill);
    GL::Renderer::setPolygonOffset(2.0f, 0.5f);

    /* Bullet setup */
    _debugDraw = BulletIntegration::DebugDraw{};
    _debugDraw.setMode(BulletIntegration::DebugDraw::Mode::DrawWireframe);
    _bWorld.setDebugDrawer(&_debugDraw);

    /* Create the ground */
    auto *ground = new RigidBody{&_scene, 0.0f, &_bGroundShape, _bWorld};
    new ColoredDrawable{*ground, _boxInstanceData, 0xffffff_rgbf,
                        Matrix4::scaling({4.0f, 4.0f, 4.0f}), _drawables};

    /* Create boxes with random colors */
    Deg hue = 42.0_degf;
    for (Int i = 0; i != 2; ++i) {
        for (Int j = 0; j != 2; ++j) {
            for (Int k = 0; k != 2; ++k) {
                auto *o = new RigidBody{&_scene, 1.0f, &_bBoxShape, _bWorld};
                o->rigidBody().setGravity(btVector3{0.0, -10, 0.0f});
                o->translate({i - 2.0f, j + 4.0f, k - 2.0f});
                o->syncPose();
                new ColoredDrawable{
                    *o, _boxInstanceData,
                    Color3::fromHsv({hue += 137.5_degf, 0.75f, 0.9f}),
                    Matrix4::scaling(Vector3{0.5f}), _drawables};
            }
        }
    }

    _ball = new MovingSphere{&_scene, &_bSphereShape, _bWorld};
    _ball->translate({0.0f, 4.0f, 0.0f});
    _ball->rigidBody().setFriction(1.0f);
    _ball->rigidBody().setRollingFriction(0.1f);
    _ball->rigidBody().setSpinningFriction(0.1f);

    /* Has to be done explicitly after the translate() above, as Magnum ->
       Bullet updates are implicitly done only for kinematic bodies */
    _ball->syncPose();

    new ColoredDrawable{*_ball, _sphereInstanceData, 0x220000_rgbf,
                        Matrix4::scaling(Vector3{0.5f}), _drawables};

    _gravityBox = new GravityBox(19.62f, Vector3{4.0f, 4.0f, 4.0f}, 0.0f, 0.0f,
                                 8.0f, 12.0f);

    /* Start the timer, loop at 60 Hz max */
#ifndef CORRADE_TARGET_EMSCRIPTEN
    setSwapInterval(1);
    setMinimalLoopPeriod(16);
#endif
    _timeline.start();
}

void Application::viewportEvent(ViewportEvent &event) {
    /* Resize the main framebuffer */
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    /* Relayout ImGui */
    _imgui.relayout(Vector2{event.windowSize()} / event.dpiScaling(),
                    event.windowSize(), event.framebufferSize());

    /* Recompute the camera's projection matrix */
    _camera->setViewport(event.framebufferSize());
}

void Application::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color |
                                 GL::FramebufferClear::Depth);
    _imgui.newFrame();

    /* Enable text input, if needed */
    if (ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if (!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    /* Housekeeping: remove any objects which are far away from the origin */
    for (Object3D *obj = _scene.children().first(); obj;) {
        Object3D *next = obj->nextSibling();
        if (obj->transformation().translation().dot() > 100 * 100)
            delete obj;

        obj = next;
    }

    /* Step bullet simulation */
    _bWorld.stepSimulation(_timeline.previousFrameDuration(), 5);

    /* Get position of the sphere */
    const Vector3 spherePosition =
        Vector3{_ball->rigidBody().getCenterOfMassPosition()};

    /* Get gravity and up-pointing vector */
    Vector3 upAxis;
    Vector3 gravity = _gravityBox->getGravity(spherePosition, &upAxis);

    /* Set gravity of the sphere */
    _ball->rigidBody().setGravity(btVector3{gravity});

    /* Adjust velocity of the sphere */
    _ball->adjustVelocity(_timeline, _orbitCamera->transformationMatrix(),
                          _playerInput, upAxis);

    /* Jump if needed */
    if (_desiredJump) {
        _desiredJump = false;
        _ball->jump(gravity, upAxis);
    }

    /* Keep the camera focused on the sphere */
    _orbitCamera->focus(_timeline, _cameraInput, spherePosition, upAxis);

    if (_drawCubes) {
        /* Populate instance data with transformations and colors */
        arrayResize(_boxInstanceData, 0);
        arrayResize(_sphereInstanceData, 0);
        _camera->draw(_drawables);

        _shader.setProjectionMatrix(_camera->projectionMatrix());

        /* Upload instance data to the GPU (orphaning the previous buffer
           contents) and draw all cubes in one call, and all spheres (if any)
           in another call */
        _boxInstanceBuffer.setData(_boxInstanceData,
                                   GL::BufferUsage::DynamicDraw);
        _box.setInstanceCount(_boxInstanceData.size());
        _shader.draw(_box);

        _sphereInstanceBuffer.setData(_sphereInstanceData,
                                      GL::BufferUsage::DynamicDraw);
        _sphere.setInstanceCount(_sphereInstanceData.size());
        _shader.draw(_sphere);
    }

    /* Debug draw. If drawing on top of cubes, avoid flickering by setting
       depth function to <= instead of just <. */
    if (_drawDebug) {
        if (_drawCubes)
            GL::Renderer::setDepthFunction(
                GL::Renderer::DepthFunction::LessOrEqual);

        _debugDraw.setTransformationProjectionMatrix(
            _camera->projectionMatrix() * _camera->cameraMatrix());
        _bWorld.debugDrawWorld();

        if (_drawCubes)
            GL::Renderer::setDepthFunction(GL::Renderer::DepthFunction::Less);
    }

    /* Menu for parameters */
    if (_showMenu)
        showMenu();

    /* Update application cursor */
    _imgui.updateApplicationCursor(*this);

    /* Render ImGui window */
    {
        GL::Renderer::enable(GL::Renderer::Feature::Blending);
        GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
        GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);

        _imgui.drawFrame();

        GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
        GL::Renderer::disable(GL::Renderer::Feature::Blending);
    }

    swapBuffers();
    _timeline.nextFrame();
    redraw();
}

void Application::keyPressEvent(KeyEvent &event) {
    /* Movement */
    if (event.key() == KeyEvent::Key::Up ||
        event.key() == KeyEvent::Key::W) {
        _playerInput.z() = -1.0f;
    } else if (event.key() == KeyEvent::Key::Left ||
               event.key() == KeyEvent::Key::A) {
        _playerInput.x() = -1.0f;
    } else if (event.key() == KeyEvent::Key::Down ||
               event.key() == KeyEvent::Key::S) {
        _playerInput.z() = 1.0f;
    } else if (event.key() == KeyEvent::Key::Right ||
               event.key() == KeyEvent::Key::D) {
        _playerInput.x() = 1.0f;
    } else if (event.key() == KeyEvent::Key::Space) {
        /* TODO(kleisauke): Fix jump behavior */
        /*_desiredJump ^= true;*/
    } else if (event.key() == KeyEvent::Key::F10) { /* Show menu */
        _showMenu ^= true;
    } else if (!_imgui.handleKeyPressEvent(event))
        return;

    event.setAccepted();
}

void Application::keyReleaseEvent(KeyEvent &event) {
    /* Movement */
    if (event.key() == KeyEvent::Key::Up || event.key() == KeyEvent::Key::W ||
        event.key() == KeyEvent::Key::Down || event.key() == KeyEvent::Key::S) {
        _playerInput.z() = 0.0f;
    } else if (event.key() == KeyEvent::Key::Left ||
               event.key() == KeyEvent::Key::A ||
               event.key() == KeyEvent::Key::Right ||
               event.key() == KeyEvent::Key::D) {
        _playerInput.x() = 0.0f;
    } else if (!_imgui.handleKeyReleaseEvent(event))
        return;

    event.setAccepted();
}

void Application::mousePressEvent(MouseEvent &event) {
    if (_imgui.handleMousePressEvent(event))
        event.setAccepted();
}

void Application::mouseReleaseEvent(MouseEvent &event) {
    if (_imgui.handleMouseReleaseEvent(event))
        event.setAccepted();
}

void Application::mouseMoveEvent(MouseMoveEvent &event) {
    if (_imgui.handleMouseMoveEvent(event)) {
        event.setAccepted();
        return;
    }

    constexpr const Float angleScale = 0.01f;
    const Float angleX = event.relativePosition().x() * angleScale;
    const Float angleY = event.relativePosition().y() * angleScale;
    _cameraInput = Vector2{angleY, angleX};

    event.setAccepted();
}

void Application::mouseScrollEvent(MouseScrollEvent &event) {
    const Float delta = event.offset().y();
    if (Math::abs(delta) < 1.0e-2f)
        return;

    if (_imgui.handleMouseScrollEvent(event))
        /* Prevent scrolling the page */
        event.setAccepted();
}

void Application::textInputEvent(TextInputEvent &event) {
    if (_imgui.handleTextInputEvent(event))
        event.setAccepted();
}

void Application::showMenu() {
    ImGui::SetNextWindowPos({10.0f, 10.0f}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.5f);
    ImGui::Begin("Options", nullptr);

    /* General information */
    ImGui::Text("Hide/show menu: F10");
    ImGui::Text("Rendering: %3.2f FPS", Double(ImGui::GetIO().Framerate));
    ImGui::Spacing();
    ImGui::Separator();

    /* Rendering parameters */
    if (ImGui::TreeNodeEx("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushID("Rendering");
        if (ImGui::ColorEdit3("Clear Color", _clearColor.data()))
            GL::Renderer::setClearColor(_clearColor);
        ImGui::Checkbox("Draw cubes", &_drawCubes);
        ImGui::Checkbox("Draw debug", &_drawDebug);
        ImGui::PopID();
        ImGui::TreePop();
    }

    ImGui::End();
}

}  // namespace GraphicsPlayground

MAGNUM_APPLICATION_MAIN(GraphicsPlayground::Application)
