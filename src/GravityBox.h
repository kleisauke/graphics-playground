#pragma once

#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector3.h>

namespace GraphicsPlayground {

using namespace Magnum;

class GravityBox {
 public:
    GravityBox(Float gravity, const Vector3 &boundaryDistance,
               Float innerDistance, Float innerFalloffDistance,
               Float outerDistance, Float outerFalloffDistance);

    Vector3 getGravity(const Vector3 &position);
    Vector3 getGravity(const Vector3 &position, Vector3 *upAxis);

 private:
    Float getGravityComponent(Float coordinate, Float distance) const;

    Float _gravity;

    Vector3 _boundaryDistance;
    Float _innerDistance, _innerFalloffDistance;
    Float _outerDistance, _outerFalloffDistance;

    Float _innerFalloffFactor, _outerFalloffFactor;
};

}  // namespace GraphicsPlayground
