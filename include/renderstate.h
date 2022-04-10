#pragma once
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <math/matrix.h>
#include <math/vectors.h>

#include "shader.h"
#include "camera.h"

struct RenderState {
    GLuint mvpLocation;
    GLuint textureLocation;
    CraftBlock::Mat4 projection;
    CraftBlock::Mat4 viewProjection;
    Camera camera;
    CraftBlock::Vec2 lastMouse;
    bool firstMouse = true;
    bool wireframeMode = false;
    std::shared_ptr<ShaderProgram> currentShader;
};

class Renderable {
public:
    virtual void Render(const RenderState &) = 0;
};
