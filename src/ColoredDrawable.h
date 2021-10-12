#pragma once

#include "InstanceData.h"

#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

namespace GraphicsPlayground {

using namespace Magnum;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

class ColoredDrawable : public SceneGraph::Drawable3D {
 public:
    ColoredDrawable(Object3D &object,
                    Containers::Array<InstanceData> &instanceData,
                    const Color3 &color, const Matrix4 &primitiveTransformation,
                    SceneGraph::DrawableGroup3D &drawables);

 private:
    void draw(const Matrix4 &transformation, SceneGraph::Camera3D &) override;

    Containers::Array<InstanceData> &_instanceData;
    Color3 _color;
    Matrix4 _primitiveTransformation;
};

}  // namespace GraphicsPlayground
