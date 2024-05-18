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
    renderer = new PieceRenderer();

    glfwSetWindowUserPointer(window, camera);
    glfwSetScrollCallback(window, &Camera::scrollCallback);
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
    glPolygonOffset( 1.0, 1.0 );
    glLineWidth(2);

    shader->use();
    shader->setVec3("pieceColors[0]", Pieces::colors[WHITE]);
    shader->setVec3("pieceColors[1]", Pieces::colors[RED]);
    shader->setVec3("pieceColors[2]", Pieces::colors[GREEN]);
    shader->setVec3("pieceColors[3]", Pieces::colors[PURPLE]);

    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(window)) {
        double tick = glfwGetTime();
        camera->updateMouse(window, lastTime - tick);
        renderer->updateMouse(window, lastTime - tick);
        lastTime = tick;
        // mat4x4_rotate_Y(model, model, M_PI / 120.f);

        glClearColor(.2f, .2f, .2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader->use();
        shader->setMat4("view", *camera->getViewMat());
        shader->setMat4("projection", *camera->getProjection());

        renderer->render1c(shader, 0, 0, 0, 0);
        renderer->render2c(shader, 1, 0, 0, 0, 2, RIGHT);
        renderer->render2c(shader, -1, 0, 0, 0, 3, LEFT);
        renderer->render2c(shader, 0, 0, 1, 0, 4, FRONT);
        renderer->render2c(shader, 0, 0, -1, 0, 5, BACK);
        renderer->render2c(shader, 0, -1, 0, 0, 6, DOWN);
        renderer->render2c(shader, 0, 1, 0, 0, 7, UP);
        renderer->render3c(shader, 1, 1, 0, 0, 7, 2);
        renderer->render3c(shader, -1, 1, 0, 0, 7, 3);
        renderer->render3c(shader, 1, -1, 0, 0, 6, 2);
        renderer->render3c(shader, -1, -1, 0, 0, 6, 3);
        renderer->render3c(shader, 0, 1, 1, 7, 0, 4);
        renderer->render3c(shader, 0, 1, -1, 7, 0, 5);
        renderer->render3c(shader, 0, -1, 1, 6, 0, 4);
        renderer->render3c(shader, 0, -1, -1, 6, 0, 5);
        renderer->render3c(shader, 1, 0, 1, 2, 4, 0);
        renderer->render3c(shader, -1, 0, 1, 3, 4, 0);
        renderer->render3c(shader, 1, 0, -1, 2, 5, 0);
        renderer->render3c(shader, -1, 0, -1, 3, 5, 0);

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
