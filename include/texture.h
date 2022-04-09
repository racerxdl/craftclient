#pragma once
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include "image.h"
#include "renderstate.h"

class _Texture {
private:
    GLuint tex;
    Image img;
    bool initialized = false;
    bool needsBufferUpdate = true;
    void updateBuffer();
public:
    void SetImage(Image img) { this->img = img; needsBufferUpdate = true; }
    void Use(const RenderState &rs);
};

typedef std::shared_ptr<_Texture> Texture;

Texture LoadTextureFromFile(const std::string &filename);