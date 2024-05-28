#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include "render.h"
#include "camera.h"
#include "puzzle.h"

class Window {
    public:
        Window();
        ~Window();
        void run();
        void close();
        void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    private:
        GLFWwindow *window;
        Shader *shader;
        Camera *camera;
        PuzzleRenderer *renderer;
        Puzzle *puzzle;
        double lastTime;
        bool vsync;
        static Window *current;
};

#endif // window.h
