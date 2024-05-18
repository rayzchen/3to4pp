#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>
#include <linmath.h>
#include "window.h"
#include "pieces.h"
#include "puzzle.h"
#include "shaders.h"

Window::Window() {
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

    window = glfwCreateWindow(800, 500, "3x3x3x3", NULL, NULL);
    if (!window) {
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL()) {
        exit(EXIT_FAILURE);
    }

    shader = new Shader(Shaders::vertex, Shaders::fragment);
    camera = new Camera(M_PI_4, 1.6f, 0.02, 50);
    renderer = new PuzzleRenderer();
    puzzle = new Puzzle();

    glfwSetWindowUserPointer(window, camera);
    glfwSetScrollCallback(window, &Camera::scrollCallback);
    glfwSetFramebufferSizeCallback(window, &Camera::framebufferSizeCallback);
}

void Window::run() {
    PieceMesh *mesh = new PieceMesh(Pieces::mesh2c);

    mat4x4 model;
    mat4x4_translate(model, 0, 0, 0);
    camera->setPitch(M_PI / 180 * -20);
    camera->setYaw(M_PI / 180 * -20);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
    glLineWidth(2);

    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(window)) {
        double tick = glfwGetTime();
        camera->updateMouse(window, lastTime - tick);
        renderer->updateMouse(window, lastTime - tick);
        lastTime = tick;

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader->use();
        shader->setMat4("view", *camera->getViewMat());
        shader->setMat4("projection", *camera->getProjection());
        renderer->renderPuzzle(shader, puzzle);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    destroy();
}

void Window::destroy() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::close() {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}
