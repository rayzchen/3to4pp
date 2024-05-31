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
    GYRO, TURN, ROTATE, GYRO_OUTER, GYRO_MIDDLE
} MoveType;

struct MoveEntry {
    MoveType type;
    float animLength;
    CellLocation cell; // for GYRO, TURN
    RotateDirection direction; // for TURN
    int location; // for slice gyros (-1/0/1 for middle gyros)
};

class PuzzleRenderer {
    public:
        friend class PuzzleController;
        PuzzleRenderer(Puzzle *puzzle);
        ~PuzzleRenderer();
        float getSpacing();
        void setSpacing(float spacing);
        void render1c(Shader *shader, const std::array<float, 3> pos, Color color);
        void render2c(Shader *shader, const std::array<float, 3> pos, const std::array<Color, 2> colors, CellLocation dir);
        void render3c(Shader *shader, const std::array<float, 3> pos, const std::array<Color, 3> colors);
        void render4c(Shader *shader, const std::array<float, 3> pos, const std::array<Color, 4> colors, int orientation);
        void renderPuzzle(Shader *shader);
        void renderCell(Shader *shader, const std::array<std::array<std::array<Piece, 3>, 3>, 3>& cell, float offset, std::array<int, 3> sliceFilter = {-1, -1, -1});
        void renderSlice(Shader *shader, const std::array<std::array<Piece, 3>, 3>& slice, float offset, std::array<int, 2> stripFilter = {-1, -1});
        void renderMiddleSlice(Shader *shader, bool addOffsetX, float offsetYZ, CellLocation filter = (CellLocation)-1);

        void renderCellOutline(Shader *shader, CellLocation cell);
        void updateMouse(GLFWwindow* window, double dt);
        bool updateAnimations(GLFWwindow *window, double dt, MoveEntry* entry);
        void scheduleMove(MoveEntry entry);

    private:
        Puzzle *puzzle;
        PieceMesh *meshes[4];
        float spacing;

        float sensitivity;
        float lastY;
        mat4x4 model;
        std::queue<MoveEntry> pendingMoves;
        bool animating;
        float animationSpeed;
        float animationProgress;

        void renderNoAnimation(Shader *shader);
        void renderLeftAnimation(Shader *shader, RotateDirection direction);
        void renderRightAnimation(Shader *shader, RotateDirection direction);
        void renderInnerAnimation(Shader *shader, RotateDirection direction);
        void renderOuterAnimation(Shader *shader, RotateDirection direction);
        void renderFrontBackAnimation(Shader *shader, CellLocation cell, RotateDirection direction);
        void renderUpDownAnimation(Shader *shader, CellLocation cell, RotateDirection direction);
        void renderRotateAnimation(Shader *shader, RotateDirection direction);
        void renderGyroXAnimation(Shader *shader, CellLocation cell);
        void renderGyroYAnimation(Shader *shader, CellLocation cell);
        void renderGyroZAnimation(Shader *shader, CellLocation cell);
        void renderOuterGyroAnimation(Shader *shader, int location);
        void renderPGyroAnimation(Shader *shader, int direction);
        void renderPGyroPosAnimation(Shader *shader, int direction);
        void renderPGyroDirAnimation(Shader *shader);
};

#endif // render.h
