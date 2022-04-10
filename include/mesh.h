#pragma once

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vector>
#include <map>
#include <memory>

#include "face.h"
#include "matrix.h"
#include "renderstate.h"

#pragma pack(push, 1)
struct Vertex {
    Vec3 pos;
    Vec4 color;
    Vec2 tex;
};
#pragma pack(pop)

class _Mesh: public Renderable {
private:
    bool needsRecompute;
    std::vector<Vec3> cachedPoints;
    std::vector<unsigned int> cachedIndexes;

    std::vector<Vertex> cachedVertex;

    // GL
    bool initialized = false;
    GLuint vertexBuffer = 0;
    GLuint vertexColorBuffer = 0;
    GLuint VAO = 0;
    GLuint EBO = 0;
    // helpers
    int cachePoint(const Vec3 point, const Vec4 color, const Vec2 tex);
public:
    _Mesh() {}
    ~_Mesh();
    _Mesh(const FaceList &faces): faces(faces), needsRecompute(true) {};
    void SetShowFace(int faceN, bool show) { needsRecompute = faces[faceN].render != show; faces[faceN].render = show; }
    void HideFace(int faceN) { SetShowFace(faceN, false); }
    void ShowFace(int faceN) { SetShowFace(faceN, true); }
    void SetFaceMap(int faceN, Vec2 map[4]);
    int NumFaces() const { return faces.size(); }
    void invalidate() { needsRecompute = true; }
    void recompute();
    void Render(const RenderState &);

    FaceList faces;
};

typedef std::shared_ptr<_Mesh> Mesh;

inline Mesh NewMesh(const FaceList &faces) { return std::make_shared<_Mesh>(faces); }