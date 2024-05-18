#include <glad/glad.h>
#include "render.h"
#include <iostream>

PieceMesh::PieceMesh(PieceType type) {
    length1 = type.triangles.size();
    length2 = type.edges.size();
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &faceVao);
    glGenVertexArrays(1, &edgeVao);
    glGenBuffers(1, &faceEbo);
    glGenBuffers(1, &edgeEbo);

    glBindVertexArray(faceVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, type.vertices.size() * sizeof(float), type.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, type.triangles.size() * sizeof(unsigned int), type.triangles.data(), GL_STATIC_DRAW);
    // 3 floats for XYZ, 1 float for color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(edgeVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, type.vertices.size() * sizeof(float), type.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, type.edges.size() * sizeof(unsigned int), type.edges.data(), GL_STATIC_DRAW);
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

PieceRenderer::PieceRenderer() {
    spacing = 0.0f;
    sensitivity = 0.011f;
    meshes[0] = new PieceMesh(Pieces::mesh1c);
    meshes[1] = new PieceMesh(Pieces::mesh2c);
    meshes[2] = new PieceMesh(Pieces::mesh3c);
}
float PieceRenderer::getSpacing() {
    return spacing;
}

void PieceRenderer::setSpacing(float spacing) {
    this->spacing = spacing;
    if (this->spacing < 0.0f) {
        this->spacing = 0.0f;
    } else if (this->spacing > 1.5f) {
        this->spacing = 1.5f;
    }
}

void PieceRenderer::render1c(Shader *shader, int x, int y, int z, int color) {
    shader->use();
    shader->setVec3("pieceColors[0]", Pieces::colors[color]);
    
    float scale = getSpacing() + 1.0f;
    mat4x4 model;
    mat4x4_translate(model, x * scale, y * scale, z * scale);
    shader->setMat4("model", model);

    shader->setInt("border", 0);
    meshes[0]->renderFaces();
    shader->setInt("border", 1);
    meshes[0]->renderEdges();
}

void PieceRenderer::render2c(Shader *shader, int x, int y, int z, int color1, int color2, CellLocation dir) {
    shader->use();
    shader->setVec3("pieceColors[0]", Pieces::colors[color1]);
    shader->setVec3("pieceColors[1]", Pieces::colors[color2]);
    
    float scale = getSpacing() + 1.0f;
    mat4x4 model;
    mat4x4_translate(model, x * scale, y * scale, z * scale);
    
    switch (dir) {
        case DOWN: mat4x4_rotate(model, model, 1, 0, 0, M_PI); break;
        case FRONT: mat4x4_rotate(model, model, 1, 0, 0, M_PI_2); break;
        case BACK: mat4x4_rotate(model, model, 1, 0, 0, -M_PI_2); break;
        case RIGHT: mat4x4_rotate(model, model, 0, 0, 1, -M_PI_2); break;
        case LEFT: mat4x4_rotate(model, model, 0, 0, 1, M_PI_2); break;
    }
    
    shader->setMat4("model", model);

    shader->setInt("border", 0);
    meshes[1]->renderFaces();
    shader->setInt("border", 1);
    meshes[1]->renderEdges();
}

void PieceRenderer::render3c(Shader *shader, int x, int y, int z, int color1, int color2, int color3) {
    shader->use();
    shader->setVec3("pieceColors[0]", Pieces::colors[color1]);
    shader->setVec3("pieceColors[1]", Pieces::colors[color2]);
    shader->setVec3("pieceColors[2]", Pieces::colors[color3]);
    
    float scale = getSpacing() + 1.0f;
    mat4x4 model;
    mat4x4_translate(model, x * scale, y * scale, z * scale);
    shader->setMat4("model", model);

    shader->setInt("border", 0);
    meshes[2]->renderFaces();
    shader->setInt("border", 1);
    meshes[2]->renderEdges();
}

void PieceRenderer::updateMouse(GLFWwindow* window, double dt) {
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
    if (state == GLFW_PRESS) {
        double curX, curY;
        glfwGetCursorPos(window, &curX, &curY);
        if (lastY != -1.0f) {
            setSpacing(spacing - (curY - lastY) * sensitivity);
        }
        lastY = curY;
    } else {
        lastY = -1.0f;
    }
}
