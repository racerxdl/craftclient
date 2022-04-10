#include "mesh.h"

using namespace CraftBlock;

// returns index of the cached point if exists
// if not, adds to cache and then returns the index
int _Mesh::cachePoint(const Vec3 point, const Vec4 color, const Vec2 tex) {
    int idx = cachedPoints.size();
    cachedPoints.push_back(point);
    Vertex v;
    v.pos = point;
    v.color = color;
    v.tex = tex;
    cachedVertex.push_back(v);
    return idx;
}

_Mesh::~_Mesh() {
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &EBO);
    }
}

void _Mesh::SetFaceMap(int faceN, Vec2 map[4]) {
    needsRecompute = true;
    assert(faces.size() > faceN);
    for (int i = 0; i < 4; i++) {
        faces[faceN].texCoord[i] = map[i];
    }
}

void _Mesh::recompute() {
    if (!needsRecompute) {
        return;
    }
    needsRecompute = false;
    cachedIndexes.clear();
    cachedPoints.clear();
    cachedVertex.clear();

    // Build cache
    // If the points are repeated, only store once
    for(auto &face: faces) {
        if (face.render) {
            // Face has 4 points, so we need to generate two triangles
            // 0 1 3 and 1 2 3
            // 013
            cachedIndexes.push_back(cachePoint(face.points[0], face.color[0], face.texCoord[0]));
            cachedIndexes.push_back(cachePoint(face.points[1], face.color[1], face.texCoord[1]));
            cachedIndexes.push_back(cachePoint(face.points[3], face.color[3], face.texCoord[3]));
            // 123
            cachedIndexes.push_back(cachePoint(face.points[1], face.color[1], face.texCoord[1]));
            cachedIndexes.push_back(cachePoint(face.points[2], face.color[2], face.texCoord[2]));
            cachedIndexes.push_back(cachePoint(face.points[3], face.color[3], face.texCoord[3]));
        }
    }

    if (!initialized) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &EBO);
        initialized = true;
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cachedIndexes.size() * sizeof(unsigned int), cachedIndexes.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, cachedVertex.size() * sizeof(Vertex), cachedVertex.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec4) + sizeof(Vec3)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void _Mesh::Render(const RenderState &) {
    recompute();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, cachedIndexes.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}