#include "puzzle.h"
#include <iostream>

Puzzle::Puzzle(std::array<Color, 8> scheme) {
    initCell(leftCell, ORANGE, {PURPLE, PINK, WHITE, YELLOW, GREEN, BLUE});
    initCell(rightCell, RED, {PINK, PURPLE, WHITE, YELLOW, GREEN, BLUE});
    initSlice(innerSlice, PURPLE, {WHITE, YELLOW, GREEN, BLUE});
    initSlice(outerSlice, PINK, {WHITE, YELLOW, GREEN, BLUE});

    middleSlicePos = 0;
    outerSlicePos = 1;
    topCell = {WHITE, UNUSED, UNUSED, UNUSED};
    bottomCell = {YELLOW, UNUSED, UNUSED, UNUSED};
    frontCell[0] = {GREEN, YELLOW, UNUSED, UNUSED};
    frontCell[1] = {GREEN, UNUSED, UNUSED, UNUSED};
    frontCell[2] = {GREEN, WHITE, UNUSED, UNUSED};
    backCell[0] = {BLUE, YELLOW, UNUSED, UNUSED};
    backCell[1] = {BLUE, UNUSED, UNUSED, UNUSED};
    backCell[2] = {BLUE, WHITE, UNUSED, UNUSED};
}

void Puzzle::initCell(std::array<std::array<std::array<Piece, 3>, 3>, 3>& cell, Color center, std::array<Color, 6> faces) {
    cell[1][1][1] = {center, UNUSED, UNUSED, UNUSED};
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            std::array<int, 3> pos = {1, 1, 1};
            pos[i] = 2 - j * 2;
            cell[pos[0]][pos[1]][pos[2]] = {center, faces[i * 2 + j], UNUSED, UNUSED};
        }
    }
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                std::array<int, 3> pos = {1, 1, 1};
                pos[i] = 2 - k * 2;
                pos[(i + 1) % 3] = 2 - j * 2;
                std::array<Color, 3> colors = {center, center, center};
                colors[(i + 1) % 3] = faces[2 + j + ((i<2) ? i : -1) * 2];
                colors[(i + 2) % 3] = faces[k + i * 2];
                cell[pos[0]][pos[1]][pos[2]] = {colors[0], colors[1], colors[2], UNUSED};
            }
        }
    }
    
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                std::array<int, 3> pos = {2 - i * 2, 2 - j * 2, 2 - k * 2};
                std::array<Color, 4> colors = {center, faces[2 + j], UNUSED, UNUSED};
                int orientation = (i + k) + 2*i*(1 - k);
                int flip = i * 5 + (-2 * i + 1) * ((j + k) % 2 + 2);
                colors[flip] = faces[i];
                colors[5 - flip] = faces[4 + k];
                cell[pos[0]][pos[1]][pos[2]] = {colors[0], colors[1], colors[2], colors[3]};
            }
        }
    }
}

void Puzzle::initSlice(std::array<std::array<Piece, 3>, 3>& slice, Color center, std::array<Color, 4> faces) {
    slice[1][1] = {center, UNUSED, UNUSED, UNUSED};
    
    for (int i = 1; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            std::array<int, 3> pos = {1, 1, 1};
            pos[i] = 2 - j * 2;
            slice[pos[1]][pos[2]] = {center, faces[i * 2 - 2 + j], UNUSED, UNUSED};
        }
    }
    
    for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 2; k++) {
            int i = 1;
            std::array<int, 3> pos = {1, 2 - k * 2, 2 - j * 2};
            slice[pos[1]][pos[2]] = {faces[k], center, faces[2 + j], UNUSED};
        }
    }
}

bool Puzzle::canRotateCell(CellLocation cell, RotateDirection direction) {
    switch (cell) {
        case IN:
        case OUT:
            return (direction == YZ || direction == ZY);
        case UP:
        case DOWN:
            return (direction == XZ || direction == ZX);
        case FRONT:
        case BACK:
            return (direction == XY || direction == YX);
        case LEFT:
        case RIGHT:
            return true;
    }
    return false;
}

void Puzzle::rotateCell(CellLocation cell, RotateDirection direction) {
    if (!canRotateCell(cell, direction)) return;
    switch (cell) {
        case IN:
            rotateSlice(leftCell[2], direction, 2);
            rotateSlice(innerSlice, direction, 1);
            rotateSlice(rightCell[0], direction, 0);
            break;
        case OUT:
            rotateSlice(rightCell[2], direction, 2);
            rotateSlice(outerSlice, direction, 1);
            rotateSlice(leftCell[0], direction, 0);
            break;
        case LEFT:
            if (direction == YZ || direction == ZY) {
                rotateCellX(leftCell, direction);
            } else if (direction == XZ || direction == ZX) {
                rotateCellY(leftCell, direction);
            } else {
                rotateCellZ(leftCell, direction);
            }
            break;
        case RIGHT:
            if (direction == YZ || direction == ZY) {
                rotateCellX(rightCell, direction);
            } else if (direction == XZ || direction == ZX) {
                rotateCellY(rightCell, direction);
            } else {
                rotateCellZ(rightCell, direction);
            }
            break;
    }
}

void Puzzle::rotateCellX(std::array<std::array<std::array<Piece, 3>, 3>, 3>& cell, RotateDirection direction) {
    rotateSlice(cell[0], direction, 0);
    rotateSlice(cell[1], direction, 1);
    rotateSlice(cell[2], direction, 2);
}

void Puzzle::rotateSlice(std::array<std::array<Piece, 3>, 3>& slice, RotateDirection direction, int sliceNum) {
    int front = (direction == YZ) ? 2 : 0;
    int back = 2 - front;
    Piece temp;
    
    // Rotate slice edges
    std::array<Piece*, 4> edges = {&slice[2][1], &slice[1][back], &slice[0][1], &slice[1][front]};
    temp = *edges[0];
    for (int i = 0; i < 3; i++) {
        *edges[i] = *edges[i + 1];
    }
    *edges[3] = temp;
    
    // Rotate slice corners
    std::array<Piece*, 4> corners = {&slice[2][front], &slice[2][back], &slice[0][back], &slice[0][front]};
    temp = *corners[0];
    for (int i = 0; i < 3; i++) {
        *corners[i] = *corners[i + 1];
    }
    *corners[3] = temp;
    
    switch (sliceNum) {
        case 1:
            // Fix 3c stickers
            for (int i = 0; i < 4; i++) {
                std::swap(corners[i]->a, corners[i]->c);
            }
            break;
        case 0:
        case 2:
            // Fix 3c stickers
            for (int i = 0; i < 4; i++) {
                std::swap(edges[i]->a, edges[i]->c);
            }
            
            // Fix 4c stickers
            int starting = (sliceNum / 2 + ((direction == YZ) ? 1 : 0)) % 2;
            for (int i = starting; i < 4; i += 2) {
                Color tempcol = corners[i]->b;
                corners[i]->b = corners[i]->c;
                corners[i]->c = corners[i]->d;
                corners[i]->d = tempcol;
            }
            for (int i = 1 - starting; i < 4; i += 2) {
                Color tempcol = corners[i]->b;
                corners[i]->b = corners[i]->d;
                corners[i]->d = corners[i]->c;
                corners[i]->c = tempcol;
            }
            break;
    }
}

void Puzzle::rotateCellY(std::array<std::array<std::array<Piece, 3>, 3>, 3>& cell, RotateDirection direction) {
    int right = (direction == XZ) ? 2 : 0;
    int left = 2 - right;
    Piece temp;
    
    for (int i = 0; i < 3; i++) {
        // Rotate slice edges
        std::array<Piece*, 4> edges = {&cell[2][i][1], &cell[1][i][left], &cell[0][i][1], &cell[1][i][right]};
        temp = *edges[0];
        for (int j = 0; j < 3; j++) {
            *edges[j] = *edges[j + 1];
        }
        *edges[3] = temp;
        
        // Rotate slice corners
        std::array<Piece*, 4> corners = {&cell[2][i][right], &cell[2][i][left], &cell[0][i][left], &cell[0][i][right]};
        temp = *corners[0];
        for (int j = 0; j < 3; j++) {
            *corners[j] = *corners[j + 1];
        }
        *corners[3] = temp;
        
        // Fix 3c stickers
        if (i == 1) {
            for (int j = 0; j < 4; j++) {
                std::swap(corners[j]->a, corners[j]->b);
            }
        } else {
            for (int j = 0; j < 4; j++) {
                std::swap(edges[j]->a, edges[j]->b);
            }
        }
    }
}

void Puzzle::rotateCellZ(std::array<std::array<std::array<Piece, 3>, 3>, 3>& cell, RotateDirection direction) {
    int top = (direction == XY) ? 2 : 0;
    int bottom = 2 - top;
    Piece temp;
    
    for (int i = 0; i < 3; i++) {
        // Rotate slice edges
        std::array<Piece*, 4> edges = {&cell[2][1][i], &cell[1][bottom][i], &cell[0][1][i], &cell[1][top][i]};
        temp = *edges[0];
        for (int j = 0; j < 3; j++) {
            *edges[j] = *edges[j + 1];
        }
        *edges[3] = temp;
        
        // Rotate slice corners
        std::array<Piece*, 4> corners = {&cell[2][top][i], &cell[2][bottom][i], &cell[0][bottom][i], &cell[0][top][i]};
        temp = *corners[0];
        for (int j = 0; j < 3; j++) {
            *corners[j] = *corners[j + 1];
        }
        *corners[3] = temp;
        
        // Fix 3c stickers
        if (i == 1) {
            for (int j = 0; j < 4; j++) {
                std::swap(corners[j]->b, corners[j]->c);
            }
        } else {
            for (int j = 0; j < 4; j++) {
                std::swap(edges[j]->b, edges[j]->c);
            }
            // Fix 4c stickers
            int starting = (i / 2 + ((direction == YX) ? 1 : 0)) % 2;
            for (int i = starting; i < 4; i += 2) {
                Color tempcol = corners[i]->b;
                corners[i]->b = corners[i]->c;
                corners[i]->c = corners[i]->d;
                corners[i]->d = tempcol;
            }
            for (int i = 1 - starting; i < 4; i += 2) {
                Color tempcol = corners[i]->b;
                corners[i]->b = corners[i]->d;
                corners[i]->d = corners[i]->c;
                corners[i]->c = tempcol;
            }
        }
    }
}
