/**************************************************************************
 * 3to4++ - https://github.com/rayzchen/3to4++
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

#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <linmath.h>

class Camera {
    public:
        Camera(float a_fov, float a_width, float a_height, float a_near, float a_far);
        float getYaw();
        void setYaw(float yaw);
        float getPitch();
        void setPitch(float pitch);
        float getZoom();
        void setZoom(float zoom);
        mat4x4* getViewMat();
        mat4x4* getProjection();

        void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        void framebufferSizeCallback(GLFWwindow* window, int width, int height);
        void updateMouse(GLFWwindow* window, double dt);

    private:
        mat4x4 view, projection;
        float width, height;
        float fov, aspect, near, far;
        float yaw, pitch;
        float zoom;
        void calcViewMat();
        bool recalculate;

        float sensitivity, deceleration;
        float yawVel, pitchVel;
        float lastX, lastY;
};

#endif // camera.h
