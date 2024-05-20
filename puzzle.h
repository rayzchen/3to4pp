#ifndef PUZZLE_H
#define PUZZLE_H

#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>

typedef enum : int {
    PURPLE, PINK, RED, ORANGE, YELLOW, WHITE, GREEN, BLUE, UNUSED
} Color;

typedef enum : int {
    IN, OUT, RIGHT, LEFT, UP, DOWN, FRONT, BACK
} CellLocation;

typedef enum : int {
    ZY, YZ, XZ, ZX, YX, XY
} RotateDirection;

typedef struct {
    // Unused depending on piece type
    Color a;
    Color b;
    Color c;
    Color d;
} Piece;

typedef std::array<std::array<std::array<Piece, 3>, 3>, 3> CellData;
typedef std::array<std::array<Piece, 3>, 3> SliceData;

class Puzzle {
    friend class PuzzleRenderer;
    public:
        Puzzle(std::array<Color, 8> scheme = {PURPLE, PINK, RED, ORANGE, YELLOW, WHITE, GREEN, BLUE});
        void save(std::string filename);
        static Puzzle load();
        bool canRotateCell(CellLocation cell, RotateDirection direction);
        void rotateCell(CellLocation cell, RotateDirection direction);
        void gyroOuterSlice();
        bool canGyroMiddle(int direction);

    private:
        // [x][y][z]
        CellData leftCell;
        // [x][y][z]
        CellData rightCell;
        // [y][z]
        SliceData innerSlice;
        // [y][z]
        SliceData outerSlice;
        // Measured from innerSlice, if 2 or -2 must match outerSlicePos
        int middleSlicePos;
        // Only 1 or -1
        int outerSlicePos;
        // Either FRONT or UP
        CellLocation middleSliceDir;
        Piece topCell;
        Piece bottomCell;
        std::array<Piece, 3> frontCell;
        std::array<Piece, 3> backCell;

        void initCell(CellData& cell, Color center, const std::array<Color, 6> faces);
        void initSlice(SliceData& slice, Color center, const std::array<Color, 4> faces);

        void rotateSlice(SliceData& slice, RotateDirection direction, int sliceNum);
        void rotateCellX(CellData& cell, RotateDirection direction);
        void rotateCellY(CellData& cell, RotateDirection direction);
        void rotateCellZ(CellData& cell, RotateDirection direction);
};

#endif // puzzle.h
