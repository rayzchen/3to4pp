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

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <stdlib.h>
#include <linmath.h>
#include <iostream>
#include "window.h"
#include "render.h"
#include "control.h"
#include "pieces.h"
#include "puzzle.h"
#include "gui.h"
#include "shaders.h"
#include "constants.h"
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>
#endif

#define WIDTH 960
#define HEIGHT 540

Window* Window::current;

Window::Window() {
    Window::current = this;
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 9);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(WIDTH, HEIGHT, "3to4++ | Rayz's Physical 3^4", NULL, NULL);
    if (!window) {
        showError("Failed to create window");
        exit(EXIT_FAILURE);
    }

#ifdef _WIN32
    SetClassLongPtr(
        glfwGetWin32Window(window),
        GCLP_HICON,
        (LONG_PTR)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(100))
    );
#endif

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        showError("Failed to load GL");
        exit(EXIT_FAILURE);
    }

    setCallbacks();

    modelShader = new Shader(Shaders::modelVertex, Shaders::modelFragment);
    guiShader = new Shader(Shaders::guiVertex, Shaders::guiFragment);
    camera = new Camera(M_PI_4, WIDTH, HEIGHT, 0.02, 50);
    puzzle = new Puzzle();
    renderer = new PuzzleRenderer(puzzle);
    controller = new PuzzleController(renderer);
    gui = new GuiRenderer(window, controller, WIDTH, HEIGHT);
    vsync = true;
    fullscreen = false;
    updateFlag = true;
    maxFrames = 60;
}

void Window::setCallbacks() {
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        Window::current->mouseButtonCallback(window, button, action);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        // todo: filter correctly with captureMouse()
        Window::current->setUpdateFlag();
    });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        if (Window::current->gui->captureMouse()) {
            Window::current->setUpdateFlag();
        } else {
            if (yoffset != 0) Window::current->setUpdateFlag();
            Window::current->camera->scrollCallback(window, xoffset, yoffset);
        }
    });
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        Window::current->camera->framebufferSizeCallback(window, width, height);
        Window::current->gui->framebufferSizeCallback(window, width, height);
        Window::current->draw();
    });
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        Window::current->keyCallback(window, key, scancode, action, mods);
        Window::current->controller->keyCallback(window, key, action, mods, Window::current->camera->inputFlipped());
    });
    glfwSetWindowPosCallback(window, [](GLFWwindow* window, int xpos, int ypos) {
        Window::current->windowPosCallback(window, xpos, ypos);
    });
}

void Window::setUpdateFlag() {
    updateFlag = true;
    extraFrame = true;
}

void Window::run() {
    lastTime = glfwGetTime();
    camera->setPitch(M_PI / 180 * -20);
    camera->setYaw(M_PI / 180 * -20);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);

    setUpdateFlag();
    glfwSwapInterval(0);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (!(fullscreen || maxFrames == 0)) {
            while (glfwGetTime() - lastTime < 1.0f / maxFrames) {
                glfwWaitEventsTimeout(1.0 / maxFrames - (glfwGetTime() - lastTime));
            }
        }
        double tick = glfwGetTime();
        double dt = tick - lastTime;
        lastTime = tick;
        if (renderer->updateMouse(window, dt)) setUpdateFlag();
        if (camera->updateMouse(window, dt)) setUpdateFlag();
        if (controller->updatePuzzle(window, dt)) setUpdateFlag();

        if (updateFlag) {
            if (extraFrame) {
                extraFrame = false;
            } else {
                updateFlag = false;
            }
            draw();
        } else {
            glfwWaitEvents();
            lastTime = glfwGetTime() - dt;
        }
    }
}

void Window::draw() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    modelShader->use();
    modelShader->setMat4("view", *camera->getViewMat());
    modelShader->setMat4("projection", *camera->getProjection());

    renderer->renderPuzzle(modelShader);
    if (controller->checkOutline(window, modelShader, camera->inputFlipped())) {
        setUpdateFlag();
    }
    gui->renderGui(guiShader);
    glfwSwapBuffers(window);
    glfwPollEvents();
}

Window::~Window() {
    delete modelShader;
    delete camera;
    delete renderer;
    delete puzzle;
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::close() {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static int saved_x;
    static int saved_y;
    static int saved_w;
    static int saved_h;
    if (action == GLFW_PRESS) {
        setUpdateFlag();
        if ((key == GLFW_KEY_ENTER && mods & GLFW_MOD_ALT) || key == GLFW_KEY_F11) {
            if (fullscreen) {
                glfwSetWindowMonitor(window, NULL, saved_x, saved_y, saved_w, saved_h, GLFW_DONT_CARE);
                glfwSwapInterval(0);
            } else {
                glfwGetWindowPos(window, &saved_x, &saved_y);
                glfwGetWindowSize(window, &saved_w, &saved_h);
                GLFWmonitor *monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                glfwSwapInterval(vsync);
            }
            fullscreen = !fullscreen;
        } else if (key == GLFW_KEY_H) {
            gui->toggleHelp();
        }
    }
}

void Window::windowPosCallback(GLFWwindow* window, int xpos, int ypos) {
    Window::current->draw();
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action) {
    if (gui->captureMouse()) {
        setUpdateFlag();
    } else {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            camera->setMousePressed(true);
        } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
            renderer->setMousePressed(true);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        camera->setMousePressed(false);
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
        renderer->setMousePressed(false);
    }
}
