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

#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include "render.h"
#include "control.h"
#include "camera.h"
#include "puzzle.h"
#include "gui.h"

class Window {
    public:
        Window();
        ~Window();
        void run();
        void draw();
        void close();
        void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void windowPosCallback(GLFWwindow* window, int xpos, int ypos);
        void setUpdateFlag();

    private:
        GLFWwindow *window;
        Shader *modelShader, *guiShader;
        Camera *camera;
        PuzzleRenderer *renderer;
        GuiRenderer *gui;
        PuzzleController *controller;
        Puzzle *puzzle;
        double lastTime;
        bool vsync;
        bool fullscreen;
        int maxFrames;
        bool updateFlag;
        static Window *current;
};

#endif // window.h
