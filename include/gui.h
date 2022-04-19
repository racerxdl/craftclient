#pragma once

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>

#include "renderstate.h"

class GUI: public Renderable {
private:
    nanogui::Screen *screen;
    nanogui::FormHelper *gui;
    nanogui::Window *nanoguiWindow;
    nanogui::TextBox *textBoxX;
    nanogui::TextBox *textBoxY;
    nanogui::TextBox *textBoxZ;
    nanogui::TextBox *textBoxPitch;
    nanogui::TextBox *textBoxYaw;
    nanogui::TextBox *textBoxFps;
    float x,y,z,pitch,yaw;
    float fps;

    bool needUpdate = true;

    void update();

public:
    GUI(GLFWwindow *window);
    void Render(const RenderState &);

    // callbacks
    void cbCursorPos(GLFWwindow *, double x, double y);
    void cbMouseButton(GLFWwindow *, int button, int action, int modifiers);
    void cbKey(GLFWwindow *, int key, int scancode, int action, int mods);
    void cbChar(GLFWwindow *, unsigned int codepoint);
    void cbDrop(GLFWwindow *, int count, const char **filenames);
    void cbScroll(GLFWwindow *, double x, double y);
    void cbFramebufferSize(GLFWwindow *, int width, int height);
};
