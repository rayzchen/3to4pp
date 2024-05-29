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
