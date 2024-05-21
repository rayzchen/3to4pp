#include "puzzle.h"
#include <iostream>

Puzzle::Puzzle(std::array<Color, 8> scheme) {
    initCell(leftCell, ORANGE, {PURPLE, PINK, WHITE, YELLOW, GREEN, BLUE});
    initCell(rightCell, RED, {PINK, PURPLE, WHITE, YELLOW, GREEN, BLUE});
    initSlice(innerSlice, PURPLE, {WHITE, YELLOW, GREEN, BLUE});
    initSlice(outerSlice, PINK, {WHITE, YELLOW, GREEN, BLUE});

    middleSlicePos = 0;
    outerSlicePos = 1;
    middleSliceDir = FRONT;
    topCell = {WHITE, UNUSED, UNUSED, UNUSED};
    bottomCell = {YELLOW, UNUSED, UNUSED, UNUSED};
    frontCell[0] = {GREEN, YELLOW, UNUSED, UNUSED};
    frontCell[1] = {GREEN, UNUSED, UNUSED, UNUSED};
    frontCell[2] = {GREEN, WHITE, UNUSED, UNUSED};
    backCell[0] = {BLUE, YELLOW, UNUSED, UNUSED};
    backCell[1] = {BLUE, UNUSED, UNUSED, UNUSED};
    backCell[2] = {BLUE, WHITE, UNUSED, UNUSED};
}

void Puzzle::initCell(CellData& cell, Color center, std::array<Color, 6> faces) {
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

void Puzzle::initSlice(SliceData& slice, Color center, std::array<Color, 4> faces) {
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

void Puzzle::rotateCellX(CellData& cell, RotateDirection direction) {
    rotateSlice(cell[0], direction, 0);
    rotateSlice(cell[1], direction, 1);
    rotateSlice(cell[2], direction, 2);
}

void Puzzle::rotateSlice(SliceData& slice, RotateDirection direction, int sliceNum) {
    int front = (direction == YZ) ? 2 : 0;
    int back = 2 - front;
    Piece temp;

    // Rotate slice edges
    temp = slice[2][1];
    std::array<Piece*, 5> edges = {&slice[2][1], &slice[1][back], &slice[0][1], &slice[1][front], &temp};
    for (int i = 0; i < 4; i++) {
        *edges[i] = *edges[i + 1];
    }

    // Rotate slice corners
    temp = slice[2][front];
    std::array<Piece*, 5> corners = {&slice[2][front], &slice[2][back], &slice[0][back], &slice[0][front], &temp};
    for (int i = 0; i < 4; i++) {
        *corners[i] = *corners[i + 1];
    }

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

void Puzzle::rotateCellY(CellData& cell, RotateDirection direction) {
    int right = (direction == XZ) ? 2 : 0;
    int left = 2 - right;
    Piece temp;

    for (int i = 0; i < 3; i++) {
        // Rotate slice edges
        temp = cell[2][i][1];
        std::array<Piece*, 5> edges = {&cell[2][i][1], &cell[1][i][left], &cell[0][i][1], &cell[1][i][right], &temp};
        for (int j = 0; j < 4; j++) {
            *edges[j] = *edges[j + 1];
        }

        // Rotate slice corners
        temp = cell[2][i][right];
        std::array<Piece*, 5> corners = {&cell[2][i][right], &cell[2][i][left], &cell[0][i][left], &cell[0][i][right], &temp};
        for (int j = 0; j < 4; j++) {
            *corners[j] = *corners[j + 1];
        }

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

void Puzzle::rotateCellZ(CellData& cell, RotateDirection direction) {
    int top = (direction == XY) ? 2 : 0;
    int bottom = 2 - top;
    Piece temp;

    for (int i = 0; i < 3; i++) {
        // Rotate slice edges
        temp = cell[2][1][i];
        std::array<Piece*, 5> edges = {&cell[2][1][i], &cell[1][bottom][i], &cell[0][1][i], &cell[1][top][i], &temp};
        for (int j = 0; j < 4; j++) {
            *edges[j] = *edges[j + 1];
        }

        // Rotate slice corners
        temp = cell[2][top][i];
        std::array<Piece*, 5> corners = {&cell[2][top][i], &cell[2][bottom][i], &cell[0][bottom][i], &cell[0][top][i], &temp};
        for (int j = 0; j < 4; j++) {
            *corners[j] = *corners[j + 1];
        }

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

void Puzzle::gyroCell(CellLocation cell) {
    switch (cell) {
        case RIGHT:
        case LEFT:
            gyroCellX(cell);
            break;
        case UP:
        case DOWN:
            gyroCellY(cell);
            break;
        case FRONT:
        case BACK:
            gyroCellZ(cell);
            break;
    }
}

void Puzzle::gyroCellX(CellLocation cell) {
    int parity = 1 - ((int)cell % 2) * 2;
    std::cout << "gyro" << std::endl;
    SliceData temp = leftCell[1];
    std::array<SliceData*, 5> slices = {&leftCell[1], &innerSlice, &rightCell[1], &outerSlice, &temp};
    if (cell == LEFT) std::swap(slices[1], slices[3]);
    for (int i = 0; i < 4; i++) {
        *slices[i] = *slices[i + 1];
    }
    temp = leftCell[0];
    slices = {&leftCell[0], &leftCell[2], &rightCell[0], &rightCell[2], &temp};
    if (cell == LEFT) std::swap(slices[1], slices[3]);
    for (int i = 0; i < 4; i++) {
        *slices[i] = *slices[i + 1];
    }
    middleSlicePos -= parity;
    if ((outerSlicePos == parity && middleSlicePos == -2 * parity) ||
        (outerSlicePos == -parity && middleSlicePos == -3 * parity)) {
        middleSlicePos += 4 * parity;
    }
    for (int i = 1; i < 3; i++) {
        for (int j = 0; j < 3; j += 2) {
            for (int k = 0; k < 3; k += 2) {
                std::array<int, 3> pos = {1, 1, 1};
                pos[(i + 1) % 3] = j;
                pos[(i + 2) % 3] = k;
                std::swap(leftCell[pos[0]][pos[1]][pos[2]].a, leftCell[pos[0]][pos[1]][pos[2]].c);
                std::swap(rightCell[pos[0]][pos[1]][pos[2]].a, rightCell[pos[0]][pos[1]][pos[2]].c);
            }
        }
    }
    for (int i = 0; i < 3; i += 2) {
        for (int j = 0; j < 3; j += 2) {
            for (int k = 0; k < 3; k += 2) {
                int twist = (i + j + k) % 4;
                if (twist == 2) {
                    std::swap(leftCell[i][j][k].a, leftCell[i][j][k].d);
                    std::swap(leftCell[i][j][k].d, leftCell[i][j][k].c);
                    std::swap(rightCell[i][j][k].a, rightCell[i][j][k].d);
                    std::swap(rightCell[i][j][k].d, rightCell[i][j][k].c);
                } else {
                    std::swap(leftCell[i][j][k].a, leftCell[i][j][k].c);
                    std::swap(leftCell[i][j][k].c, leftCell[i][j][k].d);
                    std::swap(rightCell[i][j][k].a, rightCell[i][j][k].c);
                    std::swap(rightCell[i][j][k].c, rightCell[i][j][k].d);
                }
            }
        }
    }
}

void Puzzle::gyroCellY(CellLocation cell) {
    // todo
}

void Puzzle::gyroCellZ(CellLocation cell) {
    // todo
}

bool Puzzle::canGyroMiddle(int direction) {
    if (outerSlicePos == 1) {
        return middleSlicePos + direction <= 2 && middleSlicePos + direction >= -1;
    } else {
        return middleSlicePos + direction <= 1 && middleSlicePos + direction >= -2;
    }
}

void Puzzle::gyroOuterSlice() {
    if (outerSlicePos * 2 == middleSlicePos) middleSlicePos *= -1;
    outerSlicePos *= -1;
}

void Puzzle::gyroMiddleSlice(int direction) {
    if (direction == 0) {
        if (middleSliceDir == UP) {
            middleSliceDir = FRONT;
        } else {
            middleSliceDir = UP;
        }
    } else {
        middleSlicePos += direction;
    }
}

void Puzzle::rotatePuzzle(RotateDirection direction) {
    rotateCellX(leftCell, direction);
    rotateCellX(rightCell, direction);
    rotateSlice(innerSlice, direction, 1);
    rotateSlice(outerSlice, direction, 1);

    Piece temp = topCell;
    std::array<Piece*, 5> edges = {&topCell, &backCell[1], &bottomCell, &frontCell[1], &temp};
    if (direction == ZY) {
        std::swap(edges[1], edges[3]);
    }
    for (int i = 0; i < 4; i++) {
        *edges[i] = *edges[i + 1];
    }

    int up = (int)direction * 2;
    int down = 2 - up;
    temp = frontCell[up];
    std::array<Piece*, 5> corners = {&frontCell[up], &backCell[up], &backCell[down], &frontCell[down], &temp};
    for (int i = 0; i < 4; i++) {
        *corners[i] = *corners[i + 1];
        std::swap(corners[i]->a, corners[i]->b);
    }

    gyroMiddleSlice(0);
}
