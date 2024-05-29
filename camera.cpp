/**************************************************************************
 * 3to4 - https://github.com/rayzchen/3to4
 *-------------------------------------------------------------------------
 * Copyright 2024 Ray Chen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 **************************************************************************/

#include <GLFW/glfw3.h>
#include <linmath.h>
#include <iostream>
#include "constants.h"
#include "camera.h"

Camera::Camera(float a_fov, float a_width, float a_height, float a_near, float a_far) {
    fov = a_fov;
    aspect = a_width / a_height;
    near = a_near;
    far = a_far;
    mat4x4_perspective(projection, fov, aspect, near, far);

    yaw = 0.0f;
    pitch = 0.0f;
    zoom = 10.0f;
    recalculate = true;

    width = a_width;
    height = a_height;
    sensitivity = 5.0f;
    deceleration = 5.0f;
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
    if (this->zoom > 25.0f) {
        this->zoom = 25.0f;
    } else if (this->zoom < 5.0f) {
        this->zoom = 5.0f;
    }
    recalculate = true;
}

void Camera::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset == 0) {
        return;
    }
    float zoom = this->getZoom();
    zoom -= yoffset * 0.5f;
    this->setZoom(zoom);
}

void Camera::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    this->width = width;
    this->height = height;
    this->aspect = (float)width / (float)height;
    mat4x4_perspective(this->projection, this->fov, this->aspect, this->near, this->far);
}

void Camera::updateMouse(GLFWwindow* window, double dt) {
    // std::cout << dt << std::endl;
    yawVel *= (1.0f - dt * deceleration);
    pitchVel *= (1.0f - dt * deceleration);
    setYaw(yaw - yawVel * dt);
    setPitch(pitch - pitchVel * dt);

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS) {
        double curX, curY;
        glfwGetCursorPos(window, &curX, &curY);
        if (lastX != -1.0f && lastY != -1.0f) {
            yawVel = (curX - lastX) * sensitivity / dt / width;
            pitchVel = (curY - lastY) * sensitivity / dt / height;
        }
        lastX = curX;
        lastY = curY;
    } else {
        lastX = -1.0f;
        lastY = -1.0f;
    }
}
