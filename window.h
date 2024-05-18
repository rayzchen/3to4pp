#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include "render.h"
#include "camera.h"
#include "puzzle.h"

class Window {
    public:
        Window();
        void run();
        void destroy();
        void close();

    private:
        GLFWwindow *window;
        Shader *shader;
        Camera *camera;
        PuzzleRenderer *renderer;
        Puzzle *puzzle;
        double lastTime;
};

#endif // window.h
