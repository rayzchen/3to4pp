#ifndef PIECES_H
#define PIECES_H

#include <vector>
#include <linmath.h>

typedef struct {
    std::vector<float> vertices;
    std::vector<unsigned int> triangles;
    std::vector<unsigned int> edges;
} PieceType;

class Pieces {
    public:
        static vec3 colors[8];
        static PieceType mesh1c;
        static PieceType mesh2c;
        static PieceType mesh3c;
        static PieceType mesh4c;
};

#endif // pieces.h
