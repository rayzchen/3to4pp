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

        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
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
