#include "object.h"

using namespace CraftBlock;

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

Object Object::CloneWithMesh() const {
    auto faces = FaceList(mesh->faces); // Copy faces
    auto obj = Object(NewMesh(faces), atlas);
    obj.rot = rot;
    obj.pos = pos;
    return obj;
}

void Object::SetTextureFromAtlas(const std::string &name) {
    assert(atlas != nullptr);

    auto bounds = atlas->GetBounds(name);

    Vec2 coords[4];
    coords[0] = Vec2{bounds.x, bounds.y};
    coords[1] = Vec2{bounds.x, bounds.w};
    coords[2] = Vec2{bounds.z, bounds.w};
    coords[3] = Vec2{bounds.z, bounds.y};

    for (int i = 0; i < mesh->NumFaces(); i++) {
        mesh->SetFaceMap(i, coords);
    }
}

void Object::SetTextureFromAtlas(int faceN, const std::string &name) {
    assert(faceN < mesh->NumFaces());
    auto bounds = atlas->GetBounds(name);

    Vec2 coords[4];
    coords[3] = Vec2{bounds.x, bounds.y};
    coords[0] = Vec2{bounds.x, bounds.w};
    coords[1] = Vec2{bounds.z, bounds.w};
    coords[2] = Vec2{bounds.z, bounds.y};

    mesh->SetFaceMap(faceN, coords);
}

void Object::SetTextureFromAtlas(const std::vector<std::string> &names) {
    int i = 0;
    for(auto n: names) {
        SetTextureFromAtlas(i, n);
        i++;
    }
}

void Object::Render(const RenderState &rs) {
    Object::Update();
    if (tex != nullptr) {
        tex->Use(rs);
    }
    Mat4 mvp = rs.viewProjection * m;
    glUniformMatrix4fv(rs.mvpLocation, 1, GL_FALSE, (const GLfloat*) mvp.AsFloatArray());
    if (faceCulling) {
        glEnable(GL_CULL_FACE); // cull face
        glCullFace(GL_BACK);    // cull back face
        glFrontFace(GL_CCW);    // GL_CCW for counter clock-wise
    } else {
        glDisable(GL_CULL_FACE);
    }
    if (wireframeMode) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    mesh->Render(rs);
    if (wireframeMode) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}