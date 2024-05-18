#ifndef RENDER_H
#define RENDER_H

#include <GLFW/glfw3.h>
#include <linmath.h>
#include <vector>
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

class PieceRenderer {
    public:
        PieceRenderer();
        float getSpacing();
        void setSpacing(float spacing);
        void render1c(Shader *shader, int x, int y, int z, int color);
        void render2c(Shader *shader, int x, int y, int z, int color1, int color2, CellLocation dir);
        void render3c(Shader *shader, int x, int y, int z, int color1, int color2, int color3);
        
        void updateMouse(GLFWwindow* window, double dt);
    
    private:
        PieceMesh *meshes[4];
        float spacing;
        
        float sensitivity;
        float lastY;
};

#endif // render.h
