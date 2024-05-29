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

#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <linmath.h>
#include <glad/gl.h>
#include <cstdlib>
#include "gui.h"

#ifdef _WIN32
const char* GuiRenderer::fontFile = "C:\\Windows\\Fonts\\segoeui.ttf";
#else
const char* GuiRenderer::fontFile = "/usr/share/fonts/truetype/arial.ttf";
#endif

// todo: replace with keybinds
std::vector<std::string> GuiRenderer::helpText = {
	"Controls:",
	"Left/Right Click + Drag - Move Camera",
	"Scroll Wheel - Zoom In / Out",
	"Middle Click + Drag - Explode Pieces",
	"W, E, R, S, D, F, C, V - Select Cell",
	"(L, U, B, F, I, R, D, O)",
	"I, K - Rotate x, x'",
	"J, L - Rotate y, y'",
	"O, U - Rotate z, z'",
	"SPACE - Gyro (Selected > I)",
	"M, COMMA, PERIOD - Move Outer Parts",
};
std::string GuiRenderer::helpHint = "Help: H";

GuiRenderer::GuiRenderer(int width, int height) {
	this->width = width;
	this->height = height;
	showHelp = false;
	mat4x4_ortho(projection, 0, width, 0, height, 0, 10);

	initGlyphs();
}

void GuiRenderer::initGlyphs() {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
	    std::cout << "Could not init freetype library" << std::endl;
        exit(EXIT_FAILURE);
	}
	FT_Face face;
	if (FT_New_Face(ft, fontFile, 0, &face)) {
	    std::cout << "Failed to load font" << std::endl;
        exit(EXIT_FAILURE);
	}
	FT_Set_Char_Size(face, 0, 14*64, 96, 96);
	lineHeight = face->size->metrics.height >> 6;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (unsigned char c = 0; c < 255; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "Failed to load glyph" << std::endl;
        	continue;
        }
    	unsigned int texture;
    	glGenTextures(1, &texture);
	    glBindTexture(GL_TEXTURE_2D, texture);
	    glTexImage2D(
	        GL_TEXTURE_2D,
	        0,
	        GL_RED,
	        face->glyph->bitmap.width,
	        face->glyph->bitmap.rows,
	        0,
	        GL_RED,
	        GL_UNSIGNED_BYTE,
	        face->glyph->bitmap.buffer
	    );
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    GlyphInfo glyph = {
	        texture,
	        {face->glyph->bitmap.width, face->glyph->bitmap.rows},
	        {face->glyph->bitmap_left, face->glyph->bitmap_top},
	        face->glyph->advance.x
	    };
	    glyphs.insert(std::pair<char, GlyphInfo>(c, glyph));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int GuiRenderer::getTextWidth(std::string text, float scale) {
	int width = 0;
    for (size_t i = 0; i < text.size(); i++) {
    	GlyphInfo glyph = glyphs[text[i]];
        width += (glyph.advance >> 6) * scale;
    }
    return width;
}

void GuiRenderer::renderText(Shader *shader, std::string text, float x, float y, float scale) {
	vec3 color = {1, 1, 1};
	shader->use();
	shader->setVec3("textColor", color);
	shader->setMat4("projection", projection);
 	glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    for (size_t i = 0; i < text.size(); i++) {
    	GlyphInfo glyph = glyphs[text[i]];

    	float xpos = x + glyph.bearing[0] * scale;
        float ypos = y - (glyph.size[1] - glyph.bearing[1]) * scale;

        float w = glyph.size[0] * scale;
        float h = glyph.size[1] * scale;

        float vertices[6][4] = {
            {xpos,     ypos + h, 0.0f, 0.0f},
            {xpos,     ypos,     0.0f, 1.0f},
            {xpos + w, ypos,     1.0f, 1.0f},

            {xpos,     ypos + h, 0.0f, 0.0f},
            {xpos + w, ypos,     1.0f, 1.0f},
            {xpos + w, ypos + h, 1.0f, 0.0f}
        };

        glBindTexture(GL_TEXTURE_2D, glyph.texture);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (glyph.advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GuiRenderer::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	this->width = width;
	this->height = height;
    glViewport(0, 0, width, height);
    mat4x4_ortho(projection, 0, width, 0, height, 0, 10);
}

void GuiRenderer::renderGui(Shader *shader) {
	if (showHelp) {
		int textWidth = getTextWidth(helpText[10], 1);
		for (size_t i = 0; i < helpText.size(); i++) {
			float x = width - 10 - textWidth;
			float y = height - 25 - i * lineHeight;
			if (i == 0) x = width - 10 - (textWidth + getTextWidth(helpText[i], 1)) / 2;
			renderText(shader, helpText[i], x, y, 1);
		}
	}

	int textWidth = getTextWidth(helpHint, 1);
	renderText(shader, helpHint, width - 10 - textWidth, 10, 1);
}

void GuiRenderer::toggleHelp() {
	showHelp = !showHelp;
}
