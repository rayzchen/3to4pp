#include <glad/glad.h>
#include "render.h"
#include <iostream>
#include <array>

void mat4x4_scale_pos(mat4x4 M, float k) {
    for (int i = 0; i < 3; i++) {
        M[3][i] *= k;
    }
}

PieceMesh::PieceMesh(PieceType type) {
    length1 = type.triangles.size();
    length2 = type.edges.size();
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &faceVao);
    glGenVertexArrays(1, &edgeVao);
    glGenBuffers(1, &faceEbo);
    glGenBuffers(1, &edgeEbo);

    glBindVertexArray(faceVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, type.vertices.size() * sizeof(float), type.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, type.triangles.size() * sizeof(unsigned int), type.triangles.data(), GL_STATIC_DRAW);
    // 3 floats for XYZ, 1 float for color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(edgeVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, type.vertices.size() * sizeof(float), type.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, type.edges.size() * sizeof(unsigned int), type.edges.data(), GL_STATIC_DRAW);
    // 3 floats for XYZ, 1 float for color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void PieceMesh::renderFaces() {
    glBindVertexArray(faceVao);
    glDrawElements(GL_TRIANGLES, length1, GL_UNSIGNED_INT, 0);
}

void PieceMesh::renderEdges() {
    glBindVertexArray(edgeVao);
    glDrawElements(GL_LINES, length2, GL_UNSIGNED_INT, 0);
}

Shader::Shader(const char *vertex, const char *fragment) {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex, NULL);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() {
    glUseProgram(program);
}

void Shader::setInt(const char *loc, int value) {
    int shaderLocation = glGetUniformLocation(program, loc);
    glUniform1i(shaderLocation, value);
}

void Shader::setVec3(const char *loc, vec3 vector) {
    int shaderLocation = glGetUniformLocation(program, loc);
    glUniform3fv(shaderLocation, 1, vector);
}

void Shader::setMat4(const char *loc, mat4x4 matrix) {
    int shaderLocation = glGetUniformLocation(program, loc);
    glUniformMatrix4fv(shaderLocation, 1, GL_FALSE, matrix[0]);
}

PuzzleRenderer::PuzzleRenderer() {
    spacing = 0.0f;
    sensitivity = 0.01f;
    animationProgress = 0.0f;
    mat4x4_identity(model);
    
    meshes[0] = new PieceMesh(Pieces::mesh1c);
    meshes[1] = new PieceMesh(Pieces::mesh2c);
    meshes[2] = new PieceMesh(Pieces::mesh3c);
    meshes[3] = new PieceMesh(Pieces::mesh4c);
}
float PuzzleRenderer::getSpacing() {
    return spacing;
}

void PuzzleRenderer::setSpacing(float spacing) {
    this->spacing = spacing;
    if (this->spacing < 0.0f) {
        this->spacing = 0.0f;
    } else if (this->spacing > 1.5f) {
        this->spacing = 1.5f;
    }
}

void PuzzleRenderer::render1c(Shader *shader, const std::array<float, 3> pos, Color color) {
    shader->use();
    shader->setVec3("pieceColors[0]", Pieces::colors[color]);
    
    float scale = getSpacing() + 1.0f;
    mat4x4 model;
    mat4x4_dup(model, this->model);
    mat4x4_translate_in_place(model, pos[0], pos[1], pos[2]);
    mat4x4_scale_pos(model, scale);
    shader->setMat4("model", model);

    shader->setInt("border", 0);
    meshes[0]->renderFaces();
    shader->setInt("border", 1);
    meshes[0]->renderEdges();
}

void PuzzleRenderer::render2c(Shader *shader, const std::array<float, 3> pos, const std::array<Color, 2> colors, CellLocation dir) {
    shader->use();
    shader->setVec3("pieceColors[0]", Pieces::colors[colors[0]]);
    shader->setVec3("pieceColors[1]", Pieces::colors[colors[1]]);
    
    float scale = getSpacing() + 1.0f;
    mat4x4 model;
    mat4x4_dup(model, this->model);
    mat4x4_translate_in_place(model, pos[0], pos[1], pos[2]);
    mat4x4_scale_pos(model, scale);
    
    switch (dir) {
        case DOWN: mat4x4_rotate(model, model, 1, 0, 0, M_PI); break;
        case RIGHT: mat4x4_rotate(model, model, 0, 0, 1, -M_PI_2); break;
        case LEFT: mat4x4_rotate(model, model, 0, 0, 1, M_PI_2); break;
        case FRONT: mat4x4_rotate(model, model, 1, 0, 0, M_PI_2); break;
        case BACK: mat4x4_rotate(model, model, 1, 0, 0, -M_PI_2); break;
    }
    
    shader->setMat4("model", model);

    shader->setInt("border", 0);
    meshes[1]->renderFaces();
    shader->setInt("border", 1);
    meshes[1]->renderEdges();
}

void PuzzleRenderer::render3c(Shader *shader, const std::array<float, 3> pos, const std::array<Color, 3> colors) {
    shader->use();
    shader->setVec3("pieceColors[0]", Pieces::colors[colors[0]]);
    shader->setVec3("pieceColors[1]", Pieces::colors[colors[1]]);
    shader->setVec3("pieceColors[2]", Pieces::colors[colors[2]]);
    
    float scale = getSpacing() + 1.0f;
    mat4x4 model;
    mat4x4_dup(model, this->model);
    mat4x4_translate_in_place(model, pos[0], pos[1], pos[2]);
    mat4x4_scale_pos(model, scale);
    shader->setMat4("model", model);

    shader->setInt("border", 0);
    meshes[2]->renderFaces();
    shader->setInt("border", 1);
    meshes[2]->renderEdges();
}

void PuzzleRenderer::render4c(Shader *shader, const std::array<float, 3> pos, const std::array<Color, 4> colors, int orientation) {
    shader->use();
    shader->setVec3("pieceColors[0]", Pieces::colors[colors[0]]);
    shader->setVec3("pieceColors[1]", Pieces::colors[colors[1]]);
    shader->setVec3("pieceColors[2]", Pieces::colors[colors[2]]);
    shader->setVec3("pieceColors[3]", Pieces::colors[colors[3]]);
    
    float scale = getSpacing() + 1.0f;
    mat4x4 model;
    mat4x4_dup(model, this->model);
    mat4x4_translate_in_place(model, pos[0], pos[1], pos[2]);
    mat4x4_scale_pos(model, scale);
    if (orientation > 3) {
        orientation -= 4;
        mat4x4_rotate(model, model, 1, 0, 0, M_PI);
        mat4x4_rotate(model, model, 0, 1, 0, -M_PI_2 * (orientation - 1));
    } else {
        mat4x4_rotate(model, model, 0, 1, 0, M_PI_2 * orientation);
    }
    shader->setMat4("model", model);

    shader->setInt("border", 0);
    meshes[3]->renderFaces();
    shader->setInt("border", 1);
    meshes[3]->renderEdges();
}

void PuzzleRenderer::updateMouse(GLFWwindow* window, double dt) {
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
    if (state == GLFW_PRESS) {
        double curX, curY;
        glfwGetCursorPos(window, &curX, &curY);
        if (lastY != -1.0f) {
            setSpacing(spacing - (curY - lastY) * sensitivity);
        }
        lastY = curY;
    } else {
        lastY = -1.0f;
    }
}

void PuzzleRenderer::renderPuzzle(Shader *shader, Puzzle *puzzle) {
    renderCell(shader, puzzle->leftCell, -2.5f);
    renderCell(shader, puzzle->rightCell, 1.5f);
    renderSlice(shader, puzzle->innerSlice, -0.5f);
    renderSlice(shader, puzzle->outerSlice, 3.5f);
    
    float offset = -0.5 + puzzle->middleSlicePos;
    render1c(shader, {offset, 2, 0}, puzzle->topCell.a);
    render1c(shader, {offset, -2, 0}, puzzle->bottomCell.a);

    render2c(shader, {offset, 1, 2}, {puzzle->frontCell[2].a, puzzle->frontCell[2].b}, UP);
    render1c(shader, {offset, 0, 2}, puzzle->frontCell[1].a);
    render2c(shader, {offset, -1, 2}, {puzzle->frontCell[0].a, puzzle->frontCell[0].b}, DOWN);

    render2c(shader, {offset, 1, -2}, {puzzle->backCell[2].a, puzzle->backCell[2].b}, UP);
    render1c(shader, {offset, 0, -2}, puzzle->backCell[1].a);
    render2c(shader, {offset, -1, -2}, {puzzle->backCell[0].a, puzzle->backCell[0].b}, DOWN);
}

void PuzzleRenderer::renderCell(Shader *shader, const std::array<std::array<std::array<Piece, 3>, 3>, 3>& cell, float offset, std::array<int, 3> sliceFilter) {
    render1c(shader, {offset, 0, 0}, cell[1][1][1].a);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            std::array<int, 3> pos = {0, 0, 0};
            pos[i] += j * -2 + 1;
            if ((sliceFilter[0] != -1 && pos[0] + 1 != sliceFilter[0]) ||
                (sliceFilter[1] != -1 && pos[1] + 1 != sliceFilter[1]) ||
                (sliceFilter[2] != -1 && pos[2] + 1 != sliceFilter[2])) {
                continue;
            }
            Piece piece = cell[pos[0] + 1][pos[1] + 1][pos[2] + 1];
            CellLocation orientation = (CellLocation)(i * 2 + j + 2);
            render2c(shader, {(float)pos[0] + offset, (float)pos[1], (float)pos[2]}, {piece.a, piece.b}, orientation);
        }
    }
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                std::array<int, 3> pos = {0, 0, 0};
                pos[i] = k * -2 + 1;
                pos[(i + 1) % 3] = j * -2 + 1;
                if ((sliceFilter[0] != -1 && pos[0] + 1 != sliceFilter[0]) ||
                    (sliceFilter[1] != -1 && pos[1] + 1 != sliceFilter[1]) ||
                    (sliceFilter[2] != -1 && pos[2] + 1 != sliceFilter[2])) {
                    continue;
                }
                Piece piece = cell[pos[0] + 1][pos[1] + 1][pos[2] + 1];
                render3c(shader, {(float)pos[0] + offset, (float)pos[1], (float)pos[2]}, {piece.a, piece.b, piece.c});
            }
        }
    }
    
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                std::array<int, 3> pos = {i * -2 + 1, j * -2 + 1, k * -2 + 1};
                int orientation = (i + k) + 2*i*(1 - k);
                if ((sliceFilter[0] != -1 && pos[0] + 1 != sliceFilter[0]) ||
                    (sliceFilter[1] != -1 && pos[1] + 1 != sliceFilter[1]) ||
                    (sliceFilter[2] != -1 && pos[2] + 1 != sliceFilter[2])) {
                    continue;
                }
                Piece piece = cell[pos[0] + 1][pos[1] + 1][pos[2] + 1];
                render4c(shader, {(float)pos[0] + offset, (float)pos[1], (float)pos[2]}, {piece.a, piece.b, piece.c, piece.d}, 4*j + orientation);
            }
        }
    }
}

void PuzzleRenderer::renderSlice(Shader *shader, const std::array<std::array<Piece, 3>, 3>& slice, float offset) {
    render1c(shader, {offset, 0, 0}, slice[1][1].a);
    for (int i = 1; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            std::array<int, 3> pos = {0, 0, 0};
            pos[i] += j * -2 + 1;
            Piece piece = slice[pos[1] + 1][pos[2] + 1];
            CellLocation orientation = (CellLocation)(i * 2 + j + 2);
            render2c(shader, {(float)pos[0] + offset, (float)pos[1], (float)pos[2]}, {piece.a, piece.b}, orientation);
        }
    }
    
    for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 2; k++) {
            std::array<int, 3> pos = {0, 0, 0};
            pos[1] = k * -2 + 1;
            pos[2] = j * -2 + 1;
            Piece piece = slice[pos[1] + 1][pos[2] + 1];
            render3c(shader, {(float)pos[0] + offset, (float)pos[1], (float)pos[2]}, {piece.a, piece.b, piece.c});
        }
    }
}

void PuzzleRenderer::updateAnimations(double dt) {
    animationProgress += dt;
}
