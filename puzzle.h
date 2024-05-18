#ifndef PUZZLE_H
#define PUZZLE_H

#include <string>
#include <vector>
#include <map>
#include <set>

typedef enum : int {
    WHITE, YELLOW, RED, ORANGE, GREEN, BLUE, PURPLE, PINK, UNUSED
} Color;

typedef enum : int {
    UP, DOWN, RIGHT, LEFT, FRONT, BACK, IN, OUT
} CellLocation;

typedef enum : int {
    XY, YX, XZ, ZX, YZ, ZY
} Direction;

typedef struct {
    // Unused depending on piece type
    Color a;
    Color b;
    Color c;
    Color d;
} Piece;

class Cell {
    public:
        Cell(Color center, std::vector<Piece*> v2c, std::vector<Piece*> v3c, std::vector<Piece*> v4c);
        // MUST be left or right cell
        void rotateCell(Direction direction);
    
    private:
        Color center;
        // U F R B L D
        Piece *pieces2c[6];
        // UF UR UB UL RF RB LB LF DF DR DB DL
        Piece *pieces3c[12];
        // UFR UBR UBL UFL DFR DBR DBL DFL
        Piece *pieces4c[8];
        
        // Specify slice by UFR
        void rotateSlice(Color face, int turns);
        void cycleEdges(int a, int b, int c, int d);
        void cycleCorners(int a, int b, int c, int d);
        void flipEdge(int edge);
        void cycleCorner(int corner, int a, int b, int c);
};

class Puzzle {
    public:
        Puzzle();
        void save(std::string filename);
        static Puzzle load();

    private:
        // Cell cells[8];
        // 8C2 + 8C3 + 8C4 = 154 pieces
        std::vector<Piece> pieces;
        std::map<std::set<Color>, Piece*> piecemap;
};

#endif // puzzle.h
