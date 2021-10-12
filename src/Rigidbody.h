#pragma once

#include <Corrade/Containers/Pointer.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <btBulletDynamicsCommon.h>

namespace GraphicsPlayground {

using namespace Magnum;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

class RigidBody : public Object3D {
 public:
    RigidBody(Object3D *parent, Float mass, btCollisionShape *bShape,
              btDynamicsWorld &bWorld);

    ~RigidBody() override;

    btRigidBody &rigidBody();

    /* needed after changing the pose from Magnum side */
    void syncPose();

 private:
    btDynamicsWorld &_bWorld;
    Containers::Pointer<btRigidBody> _bRigidBody;
};

}  // namespace GraphicsPlayground
