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

#ifndef PUZZLE_H
#define PUZZLE_H

#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>

typedef enum : int {
    PURPLE, PINK, RED, ORANGE, WHITE, YELLOW, GREEN, BLUE, UNUSED
} Color;

typedef enum : int {
    IN, OUT, RIGHT, LEFT, UP, DOWN, FRONT, BACK
} CellLocation;

typedef enum : int {
    ZY, YZ, XZ, ZX, YX, XY
} RotateDirection;

typedef std::array<std::array<Color, 3>, 3> SliceData;
typedef std::array<SliceData, 3> CellData;

class Puzzle {
    friend class PuzzleRenderer;
    friend class PuzzleController;
    public:
        static std::array<Color, 8> scheme;
        Puzzle();
        void resetPuzzle();

    private:
        std::array<CellData, 8> cells;
        void rotateSlice(int cell, int axis, int slice);
};

#endif // puzzle.h
