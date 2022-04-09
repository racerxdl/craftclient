#include "object.h"

void Object::Update() {
    if (needsMatrixUpdate) {
        needsMatrixUpdate = false;

        m.Identity();
        m.Translate(x,y,z);
        m.RotateX(roll);
        m.RotateY(pitch);
        m.RotateZ(yaw);
    }
}

void Object::Render(const RenderState &rs) {
    Object::Update();
    if (tex != nullptr) {
        tex->Use(rs);
    }
    Mat4 mvp = rs.viewProjection * m;
    glUniformMatrix4fv(rs.mvpLocation, 1, GL_FALSE, (const GLfloat*) mvp.AsFloatArray());
    mesh->Render(rs);
    glBindTexture(GL_TEXTURE_2D, 0);
}