#include <GLFW/glfw3.h>
#include <linmath.h>
#include "camera.h"

Camera::Camera(float fov, float aspect, float near, float far) {
    mat4x4_perspective(projection, fov, aspect, near, far);
    yaw = 0.0f;
    pitch = 0.0f;
    zoom = 10.0f;
    recalculate = true;

    sensitivity = 0.4f;
    yawVel = 0.0f;
    pitchVel = 0.0f;
}

mat4x4* Camera::getViewMat() {
    if (recalculate) {
        calcViewMat();
        recalculate = false;
    }
    return &view;
}

mat4x4* Camera::getProjection() {
    return &projection;
}

void Camera::calcViewMat() {
    mat4x4_identity(view);
    mat4x4_translate_in_place(view, 0, 0, -zoom);
    mat4x4_rotate_X(view, view, -pitch);
    mat4x4_rotate_Y(view, view, -yaw);
}

float Camera::getYaw() {
    return yaw;
}

void Camera::setYaw(float yaw) {
    this->yaw = fmod(yaw, 2 * M_PI);
    recalculate = true;
}

float Camera::getPitch() {
    return pitch;
}

void Camera::setPitch(float pitch) {
    this->pitch = pitch;
    if (this->pitch > M_PI_2) {
        this->pitch = M_PI_2;
    } else if (this->pitch < -M_PI_2) {
        this->pitch = -M_PI_2;
    }
    recalculate = true;
}

float Camera::getZoom() {
    return zoom;
}

void Camera::setZoom(float zoom) {
    this->zoom = zoom;
    if (this->zoom > 15.0f) {
        this->zoom = 15.0f;
    } else if (this->zoom < 3.0f) {
        this->zoom = 3.0f;
    }
    recalculate = true;
}

void Camera::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Camera *camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (yoffset == 0) {
        return;
    }
    float zoom = camera->getZoom();
    zoom -= yoffset * 0.5f;
    camera->setZoom(zoom);
}

void Camera::updateMouse(GLFWwindow* window, double dt) {
    yawVel *= 0.9f;
    pitchVel *= 0.9f;
    setYaw(yaw + yawVel * dt);
    setPitch(pitch + pitchVel * dt);

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS) {
        double curX, curY;
        glfwGetCursorPos(window, &curX, &curY);
        if (lastX != -1.0f && lastY != -1.0f) {
            yawVel = (curX - lastX) * sensitivity;
            pitchVel = (curY - lastY) * sensitivity;
        }
        lastX = curX;
        lastY = curY;
    } else {
        lastX = -1.0f;
        lastY = -1.0f;
    }
}
