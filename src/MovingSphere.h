#pragma once

#include "Rigidbody.h"

#include <Magnum/Math/Vector3.h>
#include <Magnum/Timeline.h>

namespace GraphicsPlayground {

using namespace Magnum;

class MovingSphere : public RigidBody {
 public:
    MovingSphere(Object3D *parent, btCollisionShape *bShape,
                 btDynamicsWorld &bWorld);

    void adjustVelocity(const Timeline &timeline,
                        const Matrix4 &playerInputSpace,
                        const Vector3 &playerInput, const Vector3 &upAxis);
    void jump(const Vector3 &gravity, const Vector3 &upAxis);
};

}  // namespace GraphicsPlayground
