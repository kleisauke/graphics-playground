#include "ColoredDrawable.h"

#include <Corrade/Containers/GrowableArray.h>

namespace GraphicsPlayground {

ColoredDrawable::ColoredDrawable(
    Object3D &object, Corrade::Containers::Array<InstanceData> &instanceData,
    const Color3 &color, const Matrix4 &primitiveTransformation,
    SceneGraph::DrawableGroup3D &drawables)
    : SceneGraph::Drawable3D{object, &drawables},
      _instanceData(instanceData), _color{color},
      _primitiveTransformation{primitiveTransformation} {}

void ColoredDrawable::draw(const Matrix4 &transformation,
                           SceneGraph::Camera3D &) {
    const Matrix4 t = transformation * _primitiveTransformation;
    arrayAppend(_instanceData, InPlaceInit, t, t.normalMatrix(), _color);
}

}  // namespace GraphicsPlayground
