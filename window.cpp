#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>
#include <linmath.h>
#include "window.h"
#include "pieces.h"
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

    glfwSetWindowUserPointer(window, camera);
    glfwSetScrollCallback(window, &Camera::scrollCallback);
}

void Window::run() {
    PieceMesh *mesh = new PieceMesh(
        Pieces::mesh2c.vertices,
        Pieces::mesh2c.triangles,
        Pieces::mesh2c.edges
    );

    mat4x4 model;
    mat4x4_translate(model, 0, 0, 0);
    camera->setPitch(M_PI / 180 * -20);
    camera->setYaw(M_PI / 180 * -20);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glLineWidth(2);

    shader->use();
    shader->setVec3("pieceColors[0]", Pieces::colors[0]);
    shader->setVec3("pieceColors[1]", Pieces::colors[2]);
    shader->setVec3("pieceColors[2]", Pieces::colors[4]);
    shader->setVec3("pieceColors[3]", Pieces::colors[6]);

    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(window)) {
        double tick = glfwGetTime();
        camera->updateMouse(window, lastTime - tick);
        lastTime = tick;
        // mat4x4_rotate_Y(model, model, M_PI / 120.f);

        glClearColor(.2f, .2f, .2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader->use();
        shader->setMat4("model", model);
        shader->setMat4("view", *camera->getViewMat());
        shader->setMat4("projection", *camera->getProjection());

        shader->setInt("border", 0);
        mesh->renderFaces();
        shader->setInt("border", 1);
        mesh->renderEdges();

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
