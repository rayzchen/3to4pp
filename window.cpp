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

        // White cell
        renderer->render1c(shader, {1.5, 0, 0}, WHITE);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                std::array<float, 3> pos = {0, 0, 0};
                pos[i] = j * -2 + 1;
                int cellNumber = i * 2 + j + 2;
                pos[0] += 1.5;
                renderer->render2c(shader, pos, {WHITE, (Color)cellNumber}, (CellLocation)cellNumber);
            }
        }
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    std::array<float, 3> pos = {0, 0, 0};
                    pos[i] = k * -2 + 1;
                    pos[(i + 1) % 3] = j * -2 + 1;
                    std::array<Color, 3> colors = {WHITE, WHITE, WHITE};
                    colors[(i + 1) % 3] = (Color)(4 + j + ((i<2) ? i : -1) * 2);
                    colors[(i + 2) % 3] = (Color)(2 + k + i * 2);
                    pos[0] += 1.5;
                    renderer->render3c(shader, pos, colors);
                }
            }
        }
        
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    std::array<float, 3> pos = {-2.0f * i + 1, -2.0f * j + 1, -2.0f * k + 1};
                    std::array<Color, 4> colors = {WHITE, (Color)(4 + j), GREEN, GREEN};
                    int orientation = (i + k) + 2*i*(1 - k);
                    int flip = i * 5 + (-2 * i + 1) * ((j + k) % 2 + 2);
                    colors[flip] = (Color)(2 + i);
                    colors[5 - flip] = (Color)(6 + k);
                    pos[0] += 1.5;
                    renderer->render4c(shader, pos, colors, 4*j + orientation);
                }
            }
        }
        
        // Yellow cell
        renderer->render1c(shader, {-2.5, 0, 0}, YELLOW);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                std::array<float, 3> pos = {0, 0, 0};
                pos[i] = j * -2 + 1;
                int cellNumber = i * 2 + j + 2;
                int orientation = cellNumber;
                if (i == 0) {
                    // Flip red orange
                    cellNumber = 5 - cellNumber;
                }
                pos[0] += -2.5;
                renderer->render2c(shader, pos, {YELLOW, (Color)cellNumber}, (CellLocation)orientation);
            }
        }
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    std::array<float, 3> pos = {0, 0, 0};
                    pos[i] = k * -2 + 1;
                    pos[(i + 1) % 3] = j * -2 + 1;
                    std::array<Color, 3> colors = {YELLOW, YELLOW, YELLOW};
                    colors[(i + 1) % 3] = (Color)(4 + j + ((i<2) ? i : -1) * 2);
                    colors[(i + 2) % 3] = (Color)(2 + k + i * 2);
                    pos[0] *= -1;
                    pos[0] += -2.5;
                    renderer->render3c(shader, pos, colors);
                }
            }
        }
        
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    std::array<float, 3> pos = {-2.0f * i + 1, -2.0f * j + 1, -2.0f * k + 1};
                    std::array<Color, 4> colors = {YELLOW, (Color)(4 + j), GREEN, GREEN};
                    int orientation = (i + k) + 2*i*(1 - k);
                    int flip = i * 5 + (-2 * i + 1) * ((j + k) % 2 + 2);
                    colors[flip] = (Color)(3 - i);
                    colors[5 - flip] = (Color)(6 + k);
                    pos[0] += -2.5;
                    renderer->render4c(shader, pos, colors, 4*j + orientation);
                }
            }
        }
        
        // Orange cell
        renderer->render1c(shader, {-0.5, 0, 0}, ORANGE);
        for (int i = 1; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                std::array<float, 3> pos = {-0.5, 0, 0};
                pos[i] = j * -2 + 1;
                int cellNumber = i * 2 + j + 2;
                renderer->render2c(shader, pos, {ORANGE, (Color)cellNumber}, (CellLocation)cellNumber);
            }
        }
        
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                int i = 1;
                std::array<float, 3> pos = {-0.5, k * -2.0f + 1, j * -2.0f + 1};
                std::array<Color, 3> colors = {(Color)(4 + k), ORANGE, (Color)(6 + j)};
                renderer->render3c(shader, pos, colors);
            }
        }
        
        // Red cell
        renderer->render1c(shader, {3.5, 0, 0}, RED);
        for (int i = 1; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                std::array<float, 3> pos = {3.5, 0, 0};
                pos[i] = j * -2 + 1;
                int cellNumber = i * 2 + j + 2;
                renderer->render2c(shader, pos, {RED, (Color)cellNumber}, (CellLocation)cellNumber);
            }
        }
        
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                int i = 1;
                std::array<float, 3> pos = {3.5, k * -2.0f + 1, j * -2.0f + 1};
                std::array<Color, 3> colors = {(Color)(4 + k), RED, (Color)(6 + j)};
                renderer->render3c(shader, pos, colors);
            }
        }
        
        // Exterior cells
        renderer->render1c(shader, {-0.5, 2, 0}, PINK);
        renderer->render1c(shader, {-0.5, -2, 0}, PURPLE);
        renderer->render1c(shader, {-0.5, 0, 2}, GREEN);
        renderer->render1c(shader, {-0.5, 0, -2}, BLUE);
        
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                std::array<float, 3> pos = {-0.5, -2*j + 1, -4*i + 2};
                std::array<Color, 2> colors = {(Color)(6 + i), (Color)j};
                int orientation = 4 + j;
                renderer->render2c(shader, pos, colors, (CellLocation)orientation);
            }
        }
        
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
