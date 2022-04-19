#include "gui.h"

#include <sstream>

GUI::GUI(GLFWwindow *window) {
    screen = new nanogui::Screen();
    screen->initialize(window, true);
    gui = new nanogui::FormHelper(screen);
    nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Info");
    textBoxX = new nanogui::TextBox(nanoguiWindow, "0.0");
    textBoxY = new nanogui::TextBox(nanoguiWindow, "0.0");
    textBoxZ = new nanogui::TextBox(nanoguiWindow, "0.0");
    textBoxPitch = new nanogui::TextBox(nanoguiWindow, "0.0");
    textBoxYaw = new nanogui::TextBox(nanoguiWindow, "0.0");
    textBoxFps = new nanogui::TextBox(nanoguiWindow, "0.0");
    gui->addWidget("X", textBoxX);
    gui->addWidget("Y", textBoxY);
    gui->addWidget("Z", textBoxZ);
    gui->addWidget("Pitch", textBoxPitch);
    gui->addWidget("Yaw", textBoxYaw);
    gui->addWidget("FPS", textBoxFps);
    needUpdate = true;
    screen->setVisible(true);
    screen->performLayout();
}
void GUI::update() {
    if (needUpdate) {
        std::stringstream s;
        s << x;
        textBoxX->setValue(s.str());
        s.str("");
        s << y;
        textBoxY->setValue(s.str());
        s.str("");
        s << z;
        textBoxZ->setValue(s.str());
        s.str("");
        s << pitch;
        textBoxPitch->setValue(s.str());
        s.str("");
        s << yaw;
        textBoxYaw->setValue(s.str());
        s.str("");
        s << fps;
        textBoxFps->setValue(s.str());
        s.str("");

        screen->performLayout();
        needUpdate = false;
    }
}
void GUI::Render(const RenderState &rs) {
    if (x != rs.camera.x) {
        needUpdate = true;
        x = rs.camera.x;
    }
    if (y != rs.camera.y) {
        needUpdate = true;
        y = rs.camera.y;
    }
    if (z != rs.camera.z) {
        needUpdate = true;
        z = rs.camera.z;
    }
    if (pitch != rs.camera.pitch) {
        needUpdate = true;
        pitch = rs.camera.pitch;
    }
    if (yaw != rs.camera.yaw) {
        needUpdate = true;
        yaw = rs.camera.yaw;
    }
    if (fps != rs.fps) {
        needUpdate = true;
        fps = rs.fps;
    }
    update();

    screen->drawContents();
    screen->drawWidgets();
}

void GUI::cbCursorPos(GLFWwindow *, double x, double y) {
    screen->cursorPosCallbackEvent(x, y);
}
void GUI::cbMouseButton(GLFWwindow *, int button, int action, int modifiers) {
    screen->mouseButtonCallbackEvent(button, action, modifiers);
}
void GUI::cbKey(GLFWwindow *, int key, int scancode, int action, int mods) {
    screen->keyCallbackEvent(key, scancode, action, mods);
}
void GUI::cbChar(GLFWwindow *, unsigned int codepoint) {
    screen->charCallbackEvent(codepoint);
}
void GUI::cbDrop(GLFWwindow *, int count, const char **filenames) {
    screen->dropCallbackEvent(count, filenames);
}
void GUI::cbScroll(GLFWwindow *, double x, double y) {
    screen->scrollCallbackEvent(x, y);
}
void GUI::cbFramebufferSize(GLFWwindow *, int width, int height) {
    screen->resizeCallbackEvent(width, height);
}