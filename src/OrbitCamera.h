#pragma once

#include <Magnum/Math/Quaternion.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/Timeline.h>

namespace GraphicsPlayground {

using namespace Magnum;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

class OrbitCamera : public Object3D {
 public:
    explicit OrbitCamera(Object3D *parent);

    void focus(const Timeline &timeline, const Vector2 &cameraInput,
               const Vector3 &focusPoint, const Vector3 &upAxis);

 private:
    void updateGravityAlignment(const Timeline &timeline,
                                const Vector3 &upAxis);
    void updateFocusPoint(const Timeline &timeline, const Vector3 &targetPoint);
    bool manualRotation(const Timeline &timeline, const Vector2 &cameraInput);
    bool automaticRotation(const Timeline &timeline);

    void updateOrbitRotation();
    void constrainAngles();

    Vector3 focusPoint, previousFocusPoint;

    Vector2 orbitAngles{-45.0f, 0.0f};

    Float lastManualRotationTime;

    Quaternion gravityAlignment{Math::IdentityInit};

    Quaternion orbitRotation;
};

}  // namespace GraphicsPlayground
