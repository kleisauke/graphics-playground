#include "Rigidbody.h"

#include <Magnum/BulletIntegration/Integration.h>
#include <Magnum/BulletIntegration/MotionState.h>

namespace GraphicsPlayground {

RigidBody::RigidBody(Object3D *parent, Float mass, btCollisionShape *bShape,
                     btDynamicsWorld &bWorld)
    : Object3D{parent}, _bWorld(bWorld) {
    /* Calculate inertia so the object reacts as it should with
       rotation and everything */
    btVector3 bInertia(0.0f, 0.0f, 0.0f);
    if (!Math::TypeTraits<Float>::equals(mass, 0.0f))
        bShape->calculateLocalInertia(mass, bInertia);

    /* Bullet rigid body setup */
    auto *motionState = new BulletIntegration::MotionState{*this};
    _bRigidBody.emplace(btRigidBody::btRigidBodyConstructionInfo{
        mass, &motionState->btMotionState(), bShape, bInertia});
    _bRigidBody->forceActivationState(DISABLE_DEACTIVATION);
    _bRigidBody->setFlags(BT_DISABLE_WORLD_GRAVITY |
                          BT_ENABLE_GYROSCOPIC_FORCE_IMPLICIT_BODY);
    bWorld.addRigidBody(_bRigidBody.get());
}

RigidBody::~RigidBody() {
    _bWorld.removeRigidBody(_bRigidBody.get());
}

btRigidBody &RigidBody::rigidBody() {
    return *_bRigidBody;
}

void RigidBody::syncPose() {
    _bRigidBody->setWorldTransform(btTransform(transformationMatrix()));
}

}  // namespace GraphicsPlayground