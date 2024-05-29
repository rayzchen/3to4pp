/**************************************************************************
 * 3to4 - https://github.com/rayzchen/3to4
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

#include "shaders.h"

const char *Shaders::vertex = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aColIdx;
out float colorIndex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    colorIndex = aColIdx;
}
)";

const char *Shaders::fragment = R"(
#version 330 core
in float colorIndex;
out vec4 FragColor;

uniform int border;
uniform vec3 pieceColors[4];

void main()
{
    if (border == 1) {
        FragColor = vec4(vec3(0.0f), 1.0f);
    } else {
        if (abs(colorIndex - 0.0f) < 0.002f) {
            FragColor = vec4(pieceColors[0], 1.0f);
        } else if (abs(colorIndex - 1.0f) < 0.002f) {
            FragColor = vec4(pieceColors[1], 1.0f);
        } else if (abs(colorIndex - 2.0f) < 0.002f) {
            FragColor = vec4(pieceColors[2], 1.0f);
        } else if (abs(colorIndex - 3.0f) < 0.002f) {
            FragColor = vec4(pieceColors[3], 1.0f);
        }
    }
}
)";
