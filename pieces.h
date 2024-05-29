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
