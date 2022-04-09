#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <linmath.h>
#include <nanogui/nanogui.h>
#include <iostream>
#include <shader.h>
#include <gui.h>
#include <mesh.h>
#include <matrix.h>
#include <object.h>
#include <renderstate.h>
#include <texture.h>
#include <meshtools.h>

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
        auto off = rs.camera.GetCamFront().CrossProduct(Vec3{0.0f, 1.0f, 0});
        off.Normalize();
        off = off * cameraSpeed;
        rs.camera.SetPos(rs.camera.GetPos() - off);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        auto off = rs.camera.GetCamFront().CrossProduct(Vec3{0.0f, 1.0f, 0});
        off.Normalize();
        off = off * cameraSpeed;
        rs.camera.SetPos(rs.camera.GetPos() + off);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        rs.camera.SetPos(rs.camera.GetPos() + Vec3{0.0f, 1.0f, 0} * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        rs.camera.SetPos(rs.camera.GetPos() - Vec3{0.0f, 1.0f, 0} * cameraSpeed);
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
    if(rs.camera.pitch > 89.0f) rs.camera.pitch =  89.0f;
    if(rs.camera.pitch < -89.0f) rs.camera.pitch = -89.0f;
}

GUI *gui = nullptr;
RenderState rs;

int main(void) {
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

    auto faces = GenerateCubeFaces(0.5, Vec3{0.5, 0.5, 0.5});
    auto texture = LoadTextureFromFile("textures/blocks/brick.png");
    auto mesh = NewMesh(faces);
    auto obj = Object(mesh, texture);
    obj.z = 5;

    auto obj2 = obj;
    obj2.x = -1;
    // obj2.roll = 30 * M_PI / 180;
    obj2.pitch = 45 * M_PI / 180;

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

    while (!glfwWindowShouldClose(window)) {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE); // cull face
        glCullFace(GL_BACK); // cull back face
        glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
        glViewport(0, 0, width, height);
        glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (rs.camera.update()) {
            rs.viewProjection = rs.projection *  rs.camera.getMatrix();
        }

        // obj.roll = 30 * M_PI / 180;
        // obj.pitch = 30 * M_PI / 180;
        // obj.yaw = (float)glfwGetTime();

        rs.currentShader->Use();

        if (rs.wireframeMode) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // WIRE FRAME MODE
        }
        obj.Render(rs);
        obj2.Render(rs);
        if (rs.wireframeMode) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); // WIRE FRAME MODE
        }

        gui->Render(rs);

        glfwSwapBuffers(window);
        glfwPollEvents();
        processInput(window, rs);
    }

    glfwDestroyWindow(window);
    return 0;
}