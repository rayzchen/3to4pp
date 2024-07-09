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

#include <glad/gl.h>
#include "render.h"
#include "control.h"
#include "constants.h"
#include <iostream>
#include <array>
#include <string>
#include <algorithm>

float smoothstep(float t) {
    return t * t * (3 - 2 * t);
}

PieceMesh::PieceMesh(PieceType type) {
    length1 = type.triangles.size();
    length2 = type.edges.size();
    normals = type.normals;
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

void PieceMesh::renderFaces(Shader* shader) {
    shader->setVec3v("normals", normals);
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
    char infoLog[1024];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        showError(infoLog);
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
        showError(infoLog);
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        showError(infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() {
    glUseProgram(program);
}

void Shader::setInt(std::string loc, int value) {
    int shaderLocation = glGetUniformLocation(program, loc.c_str());
    glUniform1i(shaderLocation, value);
}

void Shader::setFloat(std::string loc, float value) {
    int shaderLocation = glGetUniformLocation(program, loc.c_str());
    glUniform1f(shaderLocation, value);
}

void Shader::setVec3(std::string loc, vec3 vector) {
    int shaderLocation = glGetUniformLocation(program, loc.c_str());
    glUniform3fv(shaderLocation, 1, vector);
}

void Shader::setMat4(std::string loc, mat4x4 matrix) {
    int shaderLocation = glGetUniformLocation(program, loc.c_str());
    glUniformMatrix4fv(shaderLocation, 1, GL_FALSE, matrix[0]);
}

void Shader::setVec3v(std::string loc, std::vector<float> vectors) {
    int shaderLocation = glGetUniformLocation(program, loc.c_str());
    glUniform3fv(shaderLocation, vectors.size() / 3, vectors.data());
}

Shader::~Shader() {
    glDeleteProgram(program);
}

PuzzleRenderer::PuzzleRenderer(Puzzle *puzzle) {
    this->puzzle = puzzle;
    animating = false;
    animationProgress = 0.0f;
    animationSpeed = 4.0f;
    mat4x4_identity(model);
    mesh = new PieceMesh(Pieces::mesh1c);
}

void PuzzleRenderer::renderPuzzle(Shader *shader) {
    glLineWidth(2);
    if (pendingMoves.size() == 0) {
        renderNoAnimation(shader);
    } /* else if (pendingMoves.front().type == TURN) {
        MoveEntry move = pendingMoves.front();
        renderTurnAnimation(shader, move.cell, move.direction, move.slices);
    } else if (pendingMoves.front().type == GYRO) {
        MoveEntry move = pendingMoves.front();
        renderGyroAnimation(shader, move.cell);
    } */
}

void PuzzleRenderer::renderNoAnimation(Shader *shader) {
    static float a = 0.0f;
    // a += 0.1f;
    mat4x4 base;
    mat4x4_identity(base);
    mat4x4 model;
    mat4x4_identity(model);
    int rotaxis1 = 3, rotaxis2 = 2; // ZX rotation
    model[rotaxis1][rotaxis1] = std::cos(a);
    model[rotaxis1][rotaxis2] = std::sin(a);
    model[rotaxis2][rotaxis1] = -std::sin(a);
    model[rotaxis2][rotaxis2] = std::cos(a);
    int rotcell = 2, rotcellopp = 3;
    int slices = 4;

    float faceSpacing = 4.5;
    shader->use();
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 3; l++) {
                    vec3 pos = {(float)j - 1, (float)k - 1, (float)l - 1};
                    float posW;
                    int dimSwap;
                    if (i == 0) {
                        dimSwap = 3;
                        posW = -faceSpacing;
                    } else if (i == 1) {
                        dimSwap = 3;
                        posW = faceSpacing;
                    } else {
                        dimSwap = i / 2 - 1;
                        if (i % 2 == 1) {
                            pos[dimSwap] = -pos[dimSwap];
                        }
                        posW = pos[dimSwap];
                        pos[dimSwap] = faceSpacing * (i % 2 * -2 + 1);
                    }

                    if (i == rotcell || (i != rotcellopp && slices & (1 << (int)(pos[rotcell/2-1] + 1)))) {
                        shader->setMat4("model", model);
                    } else {
                        shader->setMat4("model", base);
                    }

                    shader->setFloat("posw", posW);
                    shader->setVec3("pos", pos);
                    shader->setVec3("pieceColors[0]", Pieces::colors[puzzle->cells[i][j][k][l]]);
                    shader->setInt("swap", dimSwap);
                    shader->setInt("border", 0);
                    mesh->renderFaces(shader);
                    shader->setInt("border", 1);
                    mesh->renderEdges();
                }
            }
        }
    }
}

bool PuzzleRenderer::updateAnimations(GLFWwindow* window, double dt, MoveEntry *entry) {
    if (pendingMoves.size() == 0) {
        animating = false;
    }
    if (animating) {
        animationProgress += dt * animationSpeed;
        if (animationProgress > pendingMoves.front().animLength) {
            MoveEntry lastEntry = pendingMoves.front();
            pendingMoves.pop();
            animationProgress = 0.0f;
            *entry = lastEntry;
            return true;
        }
    }
    return false;
}

void PuzzleRenderer::scheduleMove(MoveEntry entry) {
    pendingMoves.push(entry);
    animating = true;
}
