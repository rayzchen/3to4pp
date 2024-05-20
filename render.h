#ifndef RENDER_H
#define RENDER_H

#include <GLFW/glfw3.h>
#include <linmath.h>
#include <queue>
#include <array>
#include "pieces.h"
#include "puzzle.h"

class PieceMesh {
    public:
        PieceMesh(PieceType type);
        void renderFaces();
        void renderEdges();

    private:
        unsigned int vbo, faceVao, edgeVao, faceEbo, edgeEbo;
        unsigned int length1, length2;
};

class Shader {
    public:
        Shader(const char *vertex, const char *fragment);
        void use();
        void setInt(const char *loc, int value);
        void setVec3(const char *loc, vec3 vector);
        void setMat4(const char *loc, mat4x4 matrix);

    private:
        unsigned int program;
};

typedef enum {
    GYRO, TURN, ROTATE, GYRO_OUTER
} MoveType;

struct MoveEntry {
    MoveType type;
    float animLength;
    CellLocation cell; // for GYRO, TURN
    RotateDirection direction; // for TURN
    int location; // for slice gyros
};

class PuzzleRenderer {
    public:
        PuzzleRenderer(Puzzle *puzzle);
        float getSpacing();
        void setSpacing(float spacing);
        void render1c(Shader *shader, const std::array<float, 3> pos, Color color);
        void render2c(Shader *shader, const std::array<float, 3> pos, const std::array<Color, 2> colors, CellLocation dir);
        void render3c(Shader *shader, const std::array<float, 3> pos, const std::array<Color, 3> colors);
        void render4c(Shader *shader, const std::array<float, 3> pos, const std::array<Color, 4> colors, int orientation);
        void renderPuzzle(Shader *shader);
        void renderCell(Shader *shader, const std::array<std::array<std::array<Piece, 3>, 3>, 3>& cell, float offset, std::array<int, 3> sliceFilter = {-1, -1, -1});
        void renderSlice(Shader *shader, const std::array<std::array<Piece, 3>, 3>& slice, float offset);
        void renderMiddleSlice(Shader *shader, bool addOffset = true);
        
        void updateMouse(GLFWwindow* window, double dt);
        void updateAnimations(GLFWwindow *window, double dt);
        void renderNoAnimation(Shader *shader);
        void renderLeftAnimation(Shader *shader, RotateDirection direction);
        void renderRightAnimation(Shader *shader, RotateDirection direction);
        void renderOuterGyroAnimation(Shader *Shader, int location);
    
    private:
        Puzzle *puzzle;
        PieceMesh *meshes[4];
        float spacing;
        
        float sensitivity;
        float lastY;
        mat4x4 model;
        std::queue<MoveEntry> pendingMoves;
        bool animating;
        float animationProgress;
};

#endif // render.h
