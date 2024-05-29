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

#include "shaders.h"

const char *Shaders::vertex = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aColIdx;
out flat float colorIndex;
out vec3 meshPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    colorIndex = aColIdx;
    meshPos = aPos;
}
)";

const char *Shaders::fragment = R"(
#version 330 core
#define MAX_TRIANGLES 36
in flat float colorIndex;
in vec3 meshPos;
out vec4 FragColor;

uniform int border;
uniform vec3 pieceColors[4];

uniform mat4 model;
uniform vec3[MAX_TRIANGLES] normals;
uniform vec3 lightDir = vec3(-0.3f, -0.7f, -0.5f);
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

uniform int lighting = 1;

void main()
{
    if (border == 1) {
        FragColor = vec4(vec3(0.0f), 1.0f);
    } else {
        vec3 objectColor;
        if (abs(colorIndex - 0.0f) < 0.002f) {
            objectColor = pieceColors[0];
        } else if (abs(colorIndex - 1.0f) < 0.002f) {
            objectColor = pieceColors[1];
        } else if (abs(colorIndex - 2.0f) < 0.002f) {
            objectColor = pieceColors[2];
        } else if (abs(colorIndex - 3.0f) < 0.002f) {
            objectColor = pieceColors[3];
        }

        if (lighting == 1) {
#if defined(NORMAL_MAP)
            vec2 texCoord;
            vec3 tangent;
            vec3 bitangent;
            vec3 meshNormal = normals[gl_PrimitiveID];
            vec3 surfacePos = meshPos - meshNormal;
            if (abs(meshNormal.x) == 1.0f) {
                texCoord = vec2(surfacePos.z * -meshNormal.x, surfacePos.y);
                tangent = vec3(0, 0, -meshNormal.x);
            } else if (abs(meshNormal.y) == 1.0f) {
                texCoord = vec2(surfacePos.x, surfacePos.z * -meshNormal.y);
                tangent = vec3(1, 0, 0);
            } else if (abs(meshNormal.z) == 1.0f) {
                texCoord = vec2(surfacePos.x * meshNormal.z, surfacePos.y);
                tangent = vec3(meshNormal.z, 0, 0);
            }
            bitangent = cross(meshNormal, tangent);
            texCoord = (texCoord + 1) / 2;

            vec3 normalMap = normalize(vec3(0.1 - texCoord.x / 10, 0.1 - texCoord.y / 10, 1.0));
            mat3 TBN = mat3(tangent, bitangent, meshNormal);
            vec3 normal = mat3(model) * TBN * normalMap;
#else
            vec3 normal = mat3(model) * normals[gl_PrimitiveID];
#endif

            float ambientStrength = 0.8;
            vec3 ambient = ambientStrength * lightColor;

            float diff = max(dot(normal, normalize(-lightDir)), 0.0);
            vec3 diffuse = diff * lightColor;

            vec3 result = (ambient + diffuse * 0.5) * objectColor;
            FragColor = vec4(result, 1.0);
        } else {
            FragColor = vec4(objectColor, 1.0);
        }
    }
}
)";
