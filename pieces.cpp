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

#include "pieces.h"

vec3 Pieces::colors[8] = {
    {0.53f, 0.13f, 0.8f},
    {1.0f, 0.4f, 1.0f},
    {0.8f, 0.2f, 0.2f},
    {1.0f, 0.6f, 0.18f},
    {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {0.53f, 0.93f, 0.4f},
    {0.2f, 0.67f, 1.0f},
};

PieceType Pieces::mesh1c = {
    {
        // X     Y      Z       Color
        -0.5f,  0.5f, -0.5f,    0.0f,
         0.5f,  0.5f, -0.5f,    0.0f,
         0.5f, -0.5f, -0.5f,    0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f,
        -0.5f,  0.5f, 0.5f,     0.0f,
         0.5f,  0.5f, 0.5f,     0.0f,
         0.5f, -0.5f, 0.5f,     0.0f,
        -0.5f, -0.5f, 0.5f,     0.0f,
    },
    {
        0, 1, 2,
        0, 2, 3,
        4, 6, 5,
        4, 7, 6,
        0, 3, 7,
        0, 7, 4,
        1, 5, 6,
        1, 6, 2,
        0, 4, 5,
        0, 5, 1,
        3, 2, 6,
        3, 6, 7,
    },
    {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7,
    },
    {
        // X     Y      Z
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
    }
};
