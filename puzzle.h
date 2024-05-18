#ifndef PUZZLE_H
#define PUZZLE_H

#include <string>
#include <vector>
#include <map>
#include <set>

typedef enum : int {
    PURPLE, PINK, RED, ORANGE, YELLOW, WHITE, GREEN, BLUE, UNUSED
} Color;

typedef enum : int {
    IN, OUT, RIGHT, LEFT, UP, DOWN, FRONT, BACK
} CellLocation;

typedef enum : int {
    XY, YX, XZ, ZX, YZ, ZY
} RotateDirection;

typedef struct {
    // Unused depending on piece type
    Color a;
    Color b;
    Color c;
    Color d;
} Piece;

class Puzzle {
    friend class PuzzleRenderer;
    public:
        Puzzle(std::array<Color, 8> scheme = {PURPLE, PINK, RED, ORANGE, YELLOW, WHITE, GREEN, BLUE});
        void save(std::string filename);
        static Puzzle load();
        bool canRotateCell(CellLocation cell, RotateDirection direction);
        void rotateCell(CellLocation cell, RotateDirection direction);

    private:
        // [x][y][z]
        std::array<std::array<std::array<Piece, 3>, 3>, 3> leftCell;
        // [x][y][z]
        std::array<std::array<std::array<Piece, 3>, 3>, 3> rightCell;
        // [y][z]
        std::array<std::array<Piece, 3>, 3> innerSlice;
        // [y][z]
        std::array<std::array<Piece, 3>, 3> outerSlice;
        Piece topCell;
        Piece bottomCell;
        std::array<Piece, 3> frontCell;
        std::array<Piece, 3> backCell;
        
        void initCell(std::array<std::array<std::array<Piece, 3>, 3>, 3>& cell, Color center, const std::array<Color, 6> faces);
        void initSlice(std::array<std::array<Piece, 3>, 3>& slice, Color center, const std::array<Color, 4> faces);
        
        void rotateSlice(std::array<std::array<Piece, 3>, 3>& slice, RotateDirection direction, int sliceNum);
        void rotateCellX(std::array<std::array<std::array<Piece, 3>, 3>, 3>& cell, RotateDirection direction);
        void rotateCellY(std::array<std::array<std::array<Piece, 3>, 3>, 3>& cell, RotateDirection direction);
        void rotateCellZ(std::array<std::array<std::array<Piece, 3>, 3>, 3>& cell, RotateDirection direction);
};

#endif // puzzle.h
