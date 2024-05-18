#include "puzzle.h"
#include <iostream>

Puzzle::Puzzle(std::array<Color, 8> scheme) {
    initCell(leftCell, ORANGE, {PURPLE, PINK, WHITE, YELLOW, GREEN, BLUE});
    initCell(rightCell, RED, {PINK, PURPLE, WHITE, YELLOW, GREEN, BLUE});
    initSlice(innerSlice, PURPLE, {WHITE, YELLOW, GREEN, BLUE});
    initSlice(outerSlice, PINK, {WHITE, YELLOW, GREEN, BLUE});

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
