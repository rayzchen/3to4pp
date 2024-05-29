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

#ifndef GUI_H
#define GUI_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <map>
#include "render.h"

typedef struct {
    unsigned int texture;
    unsigned int size[2];
    int bearing[2];
    long int advance;
} GlyphInfo;

class GuiRenderer {
	public:
		static const char *fontFile;
		static std::vector<std::string> helpText;
		static std::string helpHint;
		GuiRenderer(int width, int height);
		void renderText(Shader *shader, std::string text, float x, float y, float scale);
		void framebufferSizeCallback(GLFWwindow* window, int width, int height);
		int getTextWidth(std::string text, float scale);
		void renderGui(Shader *shader);
		void toggleHelp();

	private:
		mat4x4 projection;
		unsigned int vao, vbo;
		std::map<char, GlyphInfo> glyphs;
		int lineHeight;
		int width, height;
		bool showHelp;

		void initGlyphs();
};

#endif // gui.h
