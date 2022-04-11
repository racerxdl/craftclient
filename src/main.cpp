#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <linmath.h>
#include <nanogui/nanogui.h>
#include <iostream>
#include <chrono>

#include <shader.h>
#include <gui.h>
#include <mesh.h>
#include <object.h>
#include <renderstate.h>
#include <texture.h>
#include <craftblock.h>
#include <mineblock.h>

using namespace CraftBlock;

void error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

void key_callback(GLFWwindow* window, RenderState &rs, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        rs.wireframeMode = !rs.wireframeMode;
    }
}

void processInput(GLFWwindow* window, RenderState &rs) {
    const float cameraSpeed = 0.05f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        rs.camera.SetPos(rs.camera.GetPos() + rs.camera.GetCamFront() * cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        rs.camera.SetPos(rs.camera.GetPos() - rs.camera.GetCamFront() * cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        auto off = rs.camera.GetCamFront().CrossProduct(Vec3{0.0f, 1.0f, 0.f});
        off.Normalize();
        off = off * cameraSpeed;
        rs.camera.SetPos(rs.camera.GetPos() - off);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        auto off = rs.camera.GetCamFront().CrossProduct(Vec3{0.0f, 1.0f, 0.f});
        off.Normalize();
        off = off * cameraSpeed;
        rs.camera.SetPos(rs.camera.GetPos() + off);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        rs.camera.SetPos(rs.camera.GetPos() + Vec3{0.0f, 1.0f, 0.f} * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        rs.camera.SetPos(rs.camera.GetPos() - Vec3{0.0f, 1.0f, 0.f} * cameraSpeed);
    }
}

void updateMouse(double xpos, double ypos, RenderState &rs) {
    if (rs.firstMouse) {
        rs.lastMouse.x = xpos;
        rs.lastMouse.y = ypos;
        rs.firstMouse = false;
    }
    float xoffset = xpos - rs.lastMouse.x;
    float yoffset = rs.lastMouse.y - ypos; // reversed since y-coordinates range from bottom to top
    rs.lastMouse.x = xpos;
    rs.lastMouse.y = ypos;

    const float sensitivity = 0.3f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    rs.camera.yaw   += xoffset;
    rs.camera.pitch += yoffset;
    if(rs.camera.pitch > 50.0f) rs.camera.pitch =  50.0f;
    if(rs.camera.pitch < -50.0f) rs.camera.pitch = -50.0f;
}

GUI *gui = nullptr;
RenderState rs;

int main(void) {
    CraftBlock::initialize();
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwSetTime(0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    window = glfwCreateWindow(1280, 720, "CraftClient", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    gui = new GUI(window);

    glfwSetCursorPosCallback(window, [](GLFWwindow *w, double x, double y) {
        gui->cbCursorPos(w,x,y);
        updateMouse(x, y, rs);
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow *w, int button, int action, int modifiers) {
        gui->cbMouseButton(w, button, action, modifiers);
    });

    glfwSetKeyCallback(window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        gui->cbKey(w, key, scancode, action, mods);
        key_callback(w, rs, key, scancode, action, mods);
    });

    glfwSetCharCallback(window,[](GLFWwindow *w, unsigned int codepoint) {
        gui->cbChar(w, codepoint);
    });

    glfwSetDropCallback(window, [](GLFWwindow *w, int count, const char **filenames) {
        gui->cbDrop(w, count, filenames);
    });

    glfwSetScrollCallback(window, [](GLFWwindow *w, double x, double y) {
        gui->cbScroll(w, x, y);
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *w, int width, int height) {
        gui->cbFramebufferSize(w, width, height);
    });

    rs.currentShader = MakeShaderProgram();
    rs.currentShader->LoadFragmentFile("shaders/fragment.glsl");
    rs.currentShader->LoadVertexFile("shaders/vertex.glsl");
    rs.currentShader->CompileShaders();

    rs.camera = Camera();
    rs.camera.SetPos({0.0f, 1.0f, 8.0f});
    rs.camera.pitch = -12;
    rs.camera.yaw = -80;

    auto faces = GenerateCubeFaces(1, Vec3{0.f, 0.f, 0.f});
    auto atlas = BuildTextureAtlasFromFiles(CraftBlock::TexturesToLoad());

    auto chunk = ChunkObject(atlas);

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            chunk.PutBlock(x, z, 0, CraftBlock::Grass);
            chunk.PutBlock(x, z, -64, CraftBlock::Grass);
            if (z > 4 && z < 8 && x > 4 && x < 8) {
                chunk.PutBlock(x, z, 4, CraftBlock::Grass);
            }
        }
    }

    chunk.PutBlock(10, 10, 3, CraftBlock::Stone);
    chunk.PutBlock(10, 11, 3, CraftBlock::Stone);
    chunk.PutBlock(10, 12, 3, CraftBlock::Stone);

    for (int i = 0; i < 7; i++) {
        chunk.PutBlock(5, 5, i, CraftBlock::Log);
    }
    chunk.PutBlock(8,  8,  1, CraftBlock::Rose);
    chunk.PutBlock(9,  9,  1, CraftBlock::TallGrass);
    chunk.PutBlock(10, 11,  1, CraftBlock::Torch);
    chunk.PutBlock(10, 11, 2, CraftBlock::Stone);
    chunk.PutBlock(10, 10, 0, CraftBlock::Water);
    chunk.PutBlock(11, 10, 0, CraftBlock::Water);
    chunk.PutBlock(12, 10, 0, CraftBlock::Water);

    rs.mvpLocation = rs.currentShader->GetUniformLocation("MVP");
    rs.textureLocation = rs.currentShader->GetUniformLocation("uSampler");

    int width, height;
    float ratio;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    rs.lastMouse.x = width / 2;
    rs.lastMouse.y = height / 2;
    rs.projection = OrthoProjection(-ratio, ratio, -1.f, 1.f, 1.f, 100.f);
    rs.projection = PerspectiveProjection((80*M_PI)/180, ratio,  0.1f, 100.f);

    auto lastFrame = std::chrono::system_clock::now();
    auto ss = std::stringstream();
    while (!glfwWindowShouldClose(window)) {
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastFrame);
        if (delta.count() != 0) {
            auto fps = 1000 / delta.count();
            ss.str("");
            ss << "CraftClient: " << fps << " FPS - " << delta.count() << "ms";
            glfwSetWindowTitle(window, ss.str().c_str());
        }
        lastFrame = std::chrono::system_clock::now();

        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, width, height);
        glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (rs.camera.update()) {
            rs.viewProjection = rs.projection *  rs.camera.getMatrix();
        }

        rs.currentShader->Use();

        chunk.Update();
        chunk.Render(rs);

        gui->Render(rs);

        glfwSwapBuffers(window);
        glfwPollEvents();
        processInput(window, rs);
        chunk.wireframeMode = rs.wireframeMode;
    }

    glfwDestroyWindow(window);
    return 0;
}