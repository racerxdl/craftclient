#pragma once

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vector>
#include <map>
#include <memory>
#include <math/matrix.h>
#include <geometry/face.h>

#include "renderstate.h"

#pragma pack(push, 1)
struct Vertex {
    CraftBlock::Vec3 pos;
    CraftBlock::Vec4 color;
    CraftBlock::Vec2 tex;
};
#pragma pack(pop)

class _Mesh: public Renderable {
private:
    bool needsRecompute;
    std::vector<CraftBlock::Vec3> cachedPoints;
    std::vector<unsigned int> cachedIndexes;

    std::vector<Vertex> cachedVertex;

    // GL
    bool initialized = false;
    GLuint vertexBuffer = 0;
    GLuint vertexColorBuffer = 0;
    GLuint VAO = 0;
    GLuint EBO = 0;
    // helpers
    int cachePoint(const CraftBlock::Vec3 point, const CraftBlock::Vec4 color, const CraftBlock::Vec2 tex);
public:
    _Mesh() {}
    ~_Mesh();
    _Mesh(const CraftBlock::FaceList &faces): faces(faces), needsRecompute(true) {};
    void SetShowFace(int faceN, bool show) { needsRecompute = faces[faceN].render != show; faces[faceN].render = show; }
    void HideFace(int faceN) { SetShowFace(faceN, false); }
    void ShowFace(int faceN) { SetShowFace(faceN, true); }
    void SetFaceMap(int faceN, CraftBlock::Vec2 map[4]);
    int NumFaces() const { return faces.size(); }
    void invalidate() { needsRecompute = true; }
    void recompute();
    void Render(const RenderState &);

    CraftBlock::FaceList faces;
};

typedef std::shared_ptr<_Mesh> Mesh;

inline Mesh NewMesh(const CraftBlock::FaceList &faces) { return std::make_shared<_Mesh>(faces); }