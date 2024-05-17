#include <glad/glad.h>
#include "render.h"
#include <iostream>

PieceMesh::PieceMesh(std::vector<float> vertices, std::vector<unsigned int> triangles, std::vector<unsigned int> edges) {
    length1 = triangles.size();
    length2 = edges.size();
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &faceVao);
    glGenVertexArrays(1, &edgeVao);
    glGenBuffers(1, &faceEbo);
    glGenBuffers(1, &edgeEbo);

    glBindVertexArray(faceVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(unsigned int), triangles.data(), GL_STATIC_DRAW);
    // 3 floats for XYZ, 1 float for color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(edgeVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(unsigned int), edges.data(), GL_STATIC_DRAW);
    // 3 floats for XYZ, 1 float for color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void PieceMesh::renderFaces() {
    glBindVertexArray(faceVao);
    glDrawElements(GL_TRIANGLES, length1, GL_UNSIGNED_INT, 0);
}

void PieceMesh::renderEdges() {
    glBindVertexArray(edgeVao);
    glDrawElements(GL_LINES, length2, GL_UNSIGNED_INT, 0);
}

Shader::Shader(const char *vertex, const char *fragment) {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex, NULL);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() {
    glUseProgram(program);
}

void Shader::setInt(const char *loc, int value) {
    int shaderLocation = glGetUniformLocation(program, loc);
    glUniform1i(shaderLocation, value);
}

void Shader::setVec3(const char *loc, vec3 vector) {
    int shaderLocation = glGetUniformLocation(program, loc);
    glUniform3fv(shaderLocation, 1, vector);
}

void Shader::setMat4(const char *loc, mat4x4 matrix) {
    int shaderLocation = glGetUniformLocation(program, loc);
    glUniformMatrix4fv(shaderLocation, 1, GL_FALSE, matrix[0]);
}
