#include "camera.h"

using namespace CraftBlock;

// https://learnopengl.com/Getting-started/Camera
bool Camera::update() {
    Vec3 up{0.0f, 1.0f, 0.0f};

    if (!needsMatrixUpdate) {
        return false;
    }
    needsMatrixUpdate = false;

    if(pitch > 89.0f) pitch =  89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    camFront.x = cos(yaw * M_PI / 180) * cos(pitch * M_PI / 180);
    camFront.y = sin(pitch * M_PI / 180);
    camFront.z = sin(yaw * M_PI / 180) * cos(pitch * M_PI / 180);
    camFront.Normalize();

    view = LookAt(pos, pos + camFront, up);

    return true;
}