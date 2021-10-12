#include "MovingSphere.h"

#include <Magnum/BulletIntegration/Integration.h>
#include <Magnum/Math/Functions.h>

namespace GraphicsPlayground {

constexpr const Float MaxSpeed = 5.0f;

constexpr const Float MaxAcceleration = 10.0f;
constexpr const Float MaxAirAcceleration = 0.0f;

constexpr const Float JumpHeight = 2.0f;

namespace {

Vector3 projectedDirectionOntoNormalized(Vector3 direction, Vector3 normal) {
    return direction - direction.projectedOntoNormalized(normal);
}

/* Returns a copy of vector with its length clamped to maxLength */
Vector3 clampLength(Vector3 vector, Float maxLength) {
    Float dot = vector.dot();
    if (dot > maxLength * maxLength) {
        Float length = std::sqrt(dot);

        Float normalizedX = vector.x() / length;
        Float normalizedY = vector.y() / length;
        Float normalizedZ = vector.z() / length;
        return Vector3{normalizedX * maxLength, 
                       normalizedY * maxLength,
                       normalizedZ * maxLength};
    }

    return vector;
}

}  // namespace

MovingSphere::MovingSphere(Object3D *parent, btCollisionShape *bShape,
                           btDynamicsWorld &bWorld)
    : RigidBody(parent, 5.0f, bShape, bWorld) {}

void MovingSphere::adjustVelocity(const Timeline &timeline,
                                  const Matrix4 &playerInputSpace,
                                  const Vector3 &playerInput,
                                  const Vector3 &upAxis) {
    Float acceleration = MaxAcceleration;
    Float speed = MaxSpeed;

    Vector3 velocity = Vector3{rigidBody().getLinearVelocity()};

    Vector3 xAxis =
        projectedDirectionOntoNormalized(playerInputSpace.right(), upAxis);
    Vector3 zAxis =
        projectedDirectionOntoNormalized(playerInputSpace.backward(), upAxis);

    Vector3 adjustment;
    adjustment.x() = playerInput.x() * speed - Math::dot(velocity, xAxis);
    adjustment.z() = playerInput.z() * speed - Math::dot(velocity, zAxis);

    adjustment = clampLength(adjustment,
                             acceleration * timeline.previousFrameDuration());

    velocity += xAxis * adjustment.x() + zAxis * adjustment.z();

    rigidBody().setLinearVelocity(btVector3{velocity});
}

void MovingSphere::jump(const Vector3 &gravity, const Vector3 &upAxis) {
    Vector3 jumpDirection = upAxis;

    Float jumpSpeed = std::sqrt(2.0f * gravity.dot() * JumpHeight);

    jumpDirection = (jumpDirection + upAxis).normalized();

    Vector3 velocity = Vector3{rigidBody().getLinearVelocity()};

    Float alignedSpeed = Math::dot(velocity, jumpDirection);
    if (alignedSpeed > 0.0f) {
        jumpSpeed = Math::max(jumpSpeed - alignedSpeed, 0.0f);
    }
    velocity += jumpDirection * jumpSpeed;

    rigidBody().setLinearVelocity(btVector3{velocity});
}

}  // namespace GraphicsPlayground
