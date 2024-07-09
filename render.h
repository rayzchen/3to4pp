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

#ifndef RENDER_H
#define RENDER_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <linmath.h>
#include <queue>
#include <array>
#include <vector>
#include "pieces.h"
#include "puzzle.h"

class Shader {
    public:
        Shader(const char *vertex, const char *fragment);
        ~Shader();
        void use();
        void setInt(std::string loc, int value);
        void setFloat(std::string loc, float value);
        void setVec3(std::string loc, vec3 vector);
        void setMat4(std::string loc, mat4x4 matrix);
        void setVec3v(std::string loc, std::vector<float> vectors);

    private:
        unsigned int program;
};

class PieceMesh {
    public:
        PieceMesh(PieceType type);
        void renderFaces(Shader* shader);
        void renderEdges();

    private:
        unsigned int vbo, faceVao, edgeVao, faceEbo, edgeEbo;
        unsigned int length1, length2;
        std::vector<float> normals;
};

typedef enum {
    GYRO, TURN
} MoveType;

struct MoveEntry {
    MoveType type;
    float animLength;
    CellLocation cell; // for GYRO, TURN
    RotateDirection direction; // for TURN
    int slices; // for TURN, bit field
};

class PuzzleRenderer {
    public:
        friend class PuzzleController;
        PuzzleRenderer(Puzzle *puzzle);
        void renderPuzzle(Shader *shader);
        bool updateAnimations(GLFWwindow *window, double dt, MoveEntry* entry);
        void scheduleMove(MoveEntry entry);

    private:
        Puzzle *puzzle;
        mat4x4 model;
        PieceMesh *mesh;
        std::queue<MoveEntry> pendingMoves;
        bool animating;
        float animationSpeed;
        float animationProgress;

        void renderNoAnimation(Shader *shader);
};

#endif // render.h
