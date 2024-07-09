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

#include "puzzle.h"
#include <iostream>

std::array<Color, 8> Puzzle::scheme = {PURPLE, PINK, RED, ORANGE, WHITE, YELLOW, GREEN, BLUE};

Puzzle::Puzzle() {
    resetPuzzle();
}

void Puzzle::resetPuzzle() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 3; l++) {
                    cells[i][j][k][l] = (Color)((k*3+l)%8);
                }
            }
        }
    }

    rotateCell(RIGHT, ZY);
}

void Puzzle::rotateSlice(int cell, int axis, int slice, int direction) {
    Color temp;
    std::array<Color*, 5> corners;
    corners[4] = &temp;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            int coord1 = 2 * ((i + j) % 2);
            int coord2 = 2 * i;
            std::array<int, 3> pos;
            pos[axis] = 2;
            pos[(axis + 1) % 3] = coord1;
            pos[(axis + 2) % 3] = coord2;
            corners[i*2+j] = &cells[cell][pos[0]][pos[1]][pos[2]];
        }
    }
    if (direction == -1) std::swap(corners[1], corners[3]);
    temp = *corners[0];
    for (int i = 0; i < 4; i++) {
        *corners[i] = *corners[i + 1];
    }

    std::array<Color*, 5> edges;
    edges[4] = &temp;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            int coord1 = 1 + j;
            int coord2 = 2 * i;
            std::array<int, 3> pos;
            pos[axis] = 2;
            pos[(axis + coord1) % 3] = coord2;
            pos[(axis + 3 - coord1) % 3] = 1;
            edges[i*2+j] = &cells[cell][pos[0]][pos[1]][pos[2]];
            std::cout << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
        }
    }
    if (direction == -1) std::swap(edges[1], edges[3]);
    temp = *edges[0];
    for (int i = 0; i < 4; i++) {
        *edges[i] = *edges[i + 1];
    }
}

int adjacentCell(int cell, int axis) {
    if (cell/2 - 1 == axis) {
        return cell % 1;
    } else if (cell/2 == 0) {
        return 1 - (cell % 2);
    }
    return (axis + 1) * 2;
}

void Puzzle::rotateCell(CellLocation cell, RotateDirection direction, int slices) {
    int axis1 = ((int)direction / 2 + 1 + (int)direction % 2) % 3;
    int axis2 = ((int)direction / 2 + 2 - (int)direction % 2) % 3;
    int cell1 = adjacentCell(cell, axis1);
    int cell2 = adjacentCell(cell, axis2);
    std::array<int, 5> rotating = {cell1, cell2, oppositeCell(cell1), oppositeCell(cell2), cell1};
    for (int i = 0; i < 4; i++) {
        rotateSlice(rotating[i], direction/2, 2);
    }
}
