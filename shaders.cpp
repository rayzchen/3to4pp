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

#ifdef __EMSCRIPTEN__
const char *Shaders::modelVertex = "#version 300 es"
#else
const char *Shaders::modelVertex = "#version 330 core"
#endif
R"(
precision mediump float;
precision mediump int;
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aColIdx;
out float colorIndex;
out vec3 meshPos;
out float fragW;

uniform vec3 pos;
uniform float posw;
uniform int swap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int outline;

void main() {
    mat4 dimSwap = mat4(1.0);
    dimSwap[swap][swap] = 0;
    dimSwap[3][3] = 0;
    dimSwap[3][swap] = 1;
    dimSwap[swap][3] = 1;
    float resigned = abs(sign(swap - 3)) * sign(pos[swap]) + (1 - abs(sign(swap - 3))) * -sign(posw);
    vec4 world = model * (dimSwap * vec4(resigned * aPos * 0.7, 0.0) + vec4(pos, posw));
    fragW = world.w;
    world = vec4(world.xyz, 3 - world.w/2);
    gl_Position = projection * view * world;
    if (outline == 1) {
        gl_Position.z -= 1e-4;
    }
    colorIndex = aColIdx;
    meshPos = aPos;
}
)";

#ifdef __EMSCRIPTEN__
const char *Shaders::modelFragment = "#version 300 es"
#else
const char *Shaders::modelFragment = "#version 330 core"
#endif
R"(
precision mediump float;
precision mediump int;
#define MAX_TRIANGLES 36
in float colorIndex;
in vec3 meshPos;
in float fragW;
out vec4 FragColor;

uniform int border;
uniform int outline;
uniform float time;
uniform vec3 pieceColors[4];

uniform mat4 model;
uniform vec3[MAX_TRIANGLES] normals;

vec3 lightDir = vec3(-0.3f, -0.7f, -0.5f);
vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

void main() {
    if (fragW > 3) discard;
    if (border == 1) {
        FragColor = vec4(vec3(0.0f), 1.0f);
    } else if (outline == 1) {
        FragColor = vec4(vec3(0.7 + 0.3 * sin(time)), 1.0f);
    }  else {
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

#if defined(LIGHTING)
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
#else
        FragColor = vec4(objectColor, 1.0);
#endif
    }
}
)";
