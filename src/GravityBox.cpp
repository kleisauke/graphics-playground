#include "GravityBox.h"

#include <Magnum/Math/Functions.h>

namespace GraphicsPlayground {

GravityBox::GravityBox(Float gravity, const Vector3 &boundaryDistance,
                       Float innerDistance, Float innerFalloffDistance,
                       Float outerDistance, Float outerFalloffDistance)
    : _gravity(gravity), _boundaryDistance(boundaryDistance),
      _innerDistance(innerDistance),
      _innerFalloffDistance(innerFalloffDistance),
      _outerDistance(outerDistance),
      _outerFalloffDistance(outerFalloffDistance) {
    _boundaryDistance = Math::max(_boundaryDistance, {});
    Float maxInner =
        Math::min(Math::min(_boundaryDistance.x(), _boundaryDistance.y()),
                  _boundaryDistance.z());
    _innerDistance = Math::min(_innerDistance, maxInner);
    _innerFalloffDistance =
        Math::max(Math::min(_innerFalloffDistance, maxInner), _innerDistance);
    _outerFalloffDistance = Math::max(_outerFalloffDistance, _outerDistance);

    _innerFalloffFactor = 1.0f / (_innerFalloffDistance - _innerDistance);
    _outerFalloffFactor = 1.0f / (_outerFalloffDistance - _outerDistance);
}

Vector3 GravityBox::getGravity(const Vector3 &position) {
    Vector3 vector{};

    int outside = 0;
    if (position.x() > _boundaryDistance.x()) {
        vector.x() = _boundaryDistance.x() - position.x();
        outside = 1;
    } else if (position.x() < -_boundaryDistance.x()) {
        vector.x() = -_boundaryDistance.x() - position.x();
        outside = 1;
    }

    if (position.y() > _boundaryDistance.y()) {
        vector.y() = _boundaryDistance.y() - position.y();
        outside += 1;
    } else if (position.y() < -_boundaryDistance.y()) {
        vector.y() = -_boundaryDistance.y() - position.y();
        outside += 1;
    }

    if (position.z() > _boundaryDistance.z()) {
        vector.z() = _boundaryDistance.z() - position.z();
        outside += 1;
    } else if (position.z() < -_boundaryDistance.z()) {
        vector.z() = -_boundaryDistance.z() - position.z();
        outside += 1;
    }

    if (outside > 0) {
        Float distance = outside == 1
                             ? Math::abs(vector.x() + vector.y() + vector.z())
                             : vector.length();
        if (distance > _outerFalloffDistance) {
            return Vector3{};
        }
        Float g = _gravity / distance;
        if (distance > _outerDistance) {
            g *= 1.0f - (distance - _outerDistance) * _outerFalloffFactor;
        }
        return g * vector;
    }

    Vector3 distances;
    distances.x() = _boundaryDistance.x() - Math::abs(position.x());
    distances.y() = _boundaryDistance.y() - Math::abs(position.y());
    distances.z() = _boundaryDistance.z() - Math::abs(position.z());

    if (distances.x() < distances.y()) {
        if (distances.x() < distances.z()) {
            vector.x() = getGravityComponent(position.x(), distances.x());
        } else {
            vector.z() = getGravityComponent(position.z(), distances.z());
        }
    } else if (distances.y() < distances.z()) {
        vector.y() = getGravityComponent(position.y(), distances.y());
    } else {
        vector.z() = getGravityComponent(position.z(), distances.z());
    }

    return vector;
}

Float GravityBox::getGravityComponent(Float coordinate, Float distance) const {
    if (distance > _innerFalloffDistance) {
        return 0.0f;
    }
    Float g = _gravity;
    if (distance > _innerDistance) {
        g *= 1.0f - (distance - _innerDistance) * _innerFalloffFactor;
    }
    return coordinate > 0.0f ? -g : g;
}

Vector3 GravityBox::getGravity(const Vector3 &position, Vector3 *upAxis) {
    Vector3 g = getGravity(position);
    if (upAxis != nullptr) {
        *upAxis = -g.normalized();
    }
    return g;
}

}  // namespace GraphicsPlayground
