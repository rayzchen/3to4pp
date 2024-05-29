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
