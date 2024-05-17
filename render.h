#ifndef RENDER_H
#define RENDER_H

#include <linmath.h>
#include <vector>

class PieceMesh {
    public:
        PieceMesh(std::vector<float> vertices, std::vector<unsigned int> triangles, std::vector<unsigned int> edges);
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

#endif // render.h
