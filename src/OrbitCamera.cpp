#include "OrbitCamera.h"

#include <Magnum/Math/Distance.h>
#include <Magnum/Math/Functions.h>

namespace GraphicsPlayground {

using namespace Math::Literals;

constexpr const Float Distance = 5.0f;

constexpr const Float FocusRadius = 1.0f;
constexpr const Float FocusCentering = 0.75f;

constexpr const Float RotationSpeed = 90.0f;

constexpr const Float MinVerticalAngle = -45.0f, MaxVerticalAngle = 60.0f;

constexpr const Float AlignDelay = 5.0f;
constexpr const Float AlignSmoothRange = 45.0f;

constexpr const Float UpAlignmentSpeed = 360.0f;

namespace {

/* Creates a rotation which rotates from u to v */
Quaternion fromToRotation(Vector3 u, Vector3 v) {
    Vector3 w = Math::cross(u, v);
    Quaternion q(w, 1.0f + Math::dot(u, v));
    return q.normalized();
}

Float getAngle(Vector2 direction) {
    Deg angle = Math::acos(direction.y());
    return direction.x() < 0.0f ? 360.0f - Float(angle) : Float(angle);
}

/* Calculates the shortest difference between two given angles */
Float deltaAngle(Float current, Float target) {
    Float delta = std::fmod(target - current, 360.0f);
    if (delta > 180.0f) {
        delta -= 360.0f;
    }
    return delta;
}

/* Moves a value current towards target */
Float moveTowards(Float current, Float target, Float maxDelta) {
    if (Math::abs(target - current) <= maxDelta) {
        return target;
    }
    return current + Math::sign(target - current) * maxDelta;
}

/* Same as moveTowards but makes sure the values interpolate correctly when they
   wrap around 360 degrees */
Float moveTowardsAngle(Float current, Float target, Float maxDelta) {
    Float delta = deltaAngle(current, target);
    if (-maxDelta < delta && delta < maxDelta) {
        return target;
    }
    target = current + delta;
    return moveTowards(current, target, maxDelta);
}

}  // namespace

OrbitCamera::OrbitCamera(Object3D *parent) : Object(parent) {
    updateOrbitRotation();
}

void OrbitCamera::focus(const Timeline &timeline, const Vector2 &cameraInput,
                        const Vector3 &targetPoint, const Vector3 &upAxis) {
    updateGravityAlignment(timeline, upAxis);
    updateFocusPoint(timeline, targetPoint);
    if (manualRotation(timeline, cameraInput) || automaticRotation(timeline)) {
        constrainAngles();
        updateOrbitRotation();
    }
    Quaternion lookRotation = gravityAlignment * orbitRotation;

    Vector3 lookDirection =
        lookRotation.transformVectorNormalized(Vector3::zAxis(-1));
    Vector3 lookPosition = focusPoint - lookDirection * Distance;
    setTransformation(Matrix4::from(lookRotation.toMatrix(), lookPosition));
}

void OrbitCamera::updateGravityAlignment(const Timeline &timeline,
                                         const Vector3 &toUp) {
    Vector3 fromUp =
        gravityAlignment.transformVectorNormalized(Vector3::yAxis());
    Deg angle = Math::angle(fromUp, toUp);
    Deg maxAngle = Deg{UpAlignmentSpeed * timeline.previousFrameDuration()};

    Quaternion newAlignment = fromToRotation(fromUp, toUp) * gravityAlignment;
    if (angle <= maxAngle) {
        gravityAlignment = newAlignment;
    } else {
        gravityAlignment =
            Math::slerp(gravityAlignment, newAlignment, maxAngle / angle);
    }
}

void OrbitCamera::updateFocusPoint(const Timeline &timeline,
                                   const Vector3 &targetPoint) {
    previousFocusPoint = focusPoint;
    if (FocusRadius > 0.0f) {
        Float distance = Math::Distance::pointPoint(targetPoint, focusPoint);
        Float t = 1.0f;
        if (distance > 0.01f && FocusCentering > 0.0f) {
            t = Math::pow(1.0f - FocusCentering,
                          timeline.previousFrameDuration());
        }
        if (distance > FocusRadius) {
            t = Math::min(t, FocusRadius / distance);
        }
        focusPoint = Math::lerp(targetPoint, focusPoint, t);
    } else {
        focusPoint = targetPoint;
    }
}

bool OrbitCamera::manualRotation(const Timeline &timeline,
                                 const Vector2 &cameraInput) {
    constexpr const Float e = 0.01f;
    if (cameraInput.x() < -e || cameraInput.x() > e || cameraInput.y() < -e ||
        cameraInput.y() > e) {
        orbitAngles +=
            RotationSpeed * timeline.previousFrameDuration() * cameraInput;
        lastManualRotationTime = timeline.previousFrameTime();
        return true;
    }
    return false;
}

bool OrbitCamera::automaticRotation(const Timeline &timeline) {
    if (timeline.previousFrameTime() - lastManualRotationTime < AlignDelay) {
        return false;
    }
    Vector3 alignedDelta =
        gravityAlignment.inverted().transformVectorNormalized(
            (focusPoint - previousFocusPoint));
    Vector2 movement{alignedDelta.x(), alignedDelta.z()};
    Float movementDeltaSqr = movement.dot();
    if (movementDeltaSqr < 0.0001f) {
        return false;
    }

    Float headingAngle = getAngle(movement / Math::sqrt(movementDeltaSqr));
    Float deltaAbs = Math::abs(deltaAngle(orbitAngles.y(), headingAngle));
    Float rotationChange =
        RotationSpeed *
        Math::min(timeline.previousFrameDuration(), movementDeltaSqr);
    if (deltaAbs < AlignSmoothRange) {
        rotationChange *= deltaAbs / AlignSmoothRange;
    } else if (180.0f - deltaAbs < AlignSmoothRange) {
        rotationChange *= (180.0f - deltaAbs) / AlignSmoothRange;
    }

    orbitAngles.y() =
        moveTowardsAngle(orbitAngles.y(), headingAngle, rotationChange);
    return true;
}
void OrbitCamera::updateOrbitRotation() {
    orbitRotation =
        Quaternion::rotation(Deg(orbitAngles.y()), Vector3::yAxis()) *
        Quaternion::rotation(Deg(orbitAngles.x()), Vector3::xAxis());
}

void OrbitCamera::constrainAngles() {
    orbitAngles.x() =
        Math::clamp(orbitAngles.x(), MinVerticalAngle, MaxVerticalAngle);

    if (orbitAngles.y() < 0.0f) {
        orbitAngles.y() += 360.0f;
    } else if (orbitAngles.y() >= 360.0f) {
        orbitAngles.y() -= 360.0f;
    }
}
}  // namespace GraphicsPlayground
