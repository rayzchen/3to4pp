#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <linmath.h>

class Camera {
    public:
        Camera(float fov, float aspect, float near, float far);
        float getYaw();
        void setYaw(float yaw);
        float getPitch();
        void setPitch(float pitch);
        float getZoom();
        void setZoom(float zoom);
        mat4x4* getViewMat();
        mat4x4* getProjection();

        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        void updateMouse(GLFWwindow* window, double dt);

    private:
        mat4x4 view, projection;
        float yaw, pitch;
        float zoom;
        void calcViewMat();
        bool recalculate;

        float sensitivity;
        float yawVel, pitchVel;
        float lastX, lastY;
};

#endif // camera.h
