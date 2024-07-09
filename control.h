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

#ifndef CONTROL_H
#define CONTROL_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
#include <random>
#include "render.h"
#include "puzzle.h"

void showError(std::string text);

class MoveHistory {
	public:
		MoveHistory();
		void reset();
		void insertMove(MoveEntry entry);
		bool isOpposite(MoveEntry entry1, MoveEntry entry2);
		MoveEntry getOpposite(MoveEntry entry);
		bool undoMove(MoveEntry* entry);
		bool redoMove(MoveEntry* entry);
		bool canUndo();
		bool canRedo();
		int getTurnCount();

	private:
		int turnCount;
		std::vector<MoveEntry> history;
		std::vector<MoveEntry> redoList;
		bool undoing;
		bool redoing;
};

class PuzzleController {
	public:
		friend class GuiRenderer;
		PuzzleController(PuzzleRenderer* renderer);
		~PuzzleController();
		bool updatePuzzle(GLFWwindow* window, double dt);
        void keyCallback(GLFWwindow* window, int key, int action, int mods, bool flip);
        std::string getStatus();
        void performMove(MoveEntry entry);
        void performScramble();
        void getScrambleTwists();

        void resetPuzzle();
        void scramblePuzzle(int scrambleLength);
        void undoMove();
        void redoMove();
        void openFile(std::string filename);

	    static int cellKeys[];
    	static int directionKeys[];

	private:
		PuzzleRenderer *renderer;
		Puzzle *puzzle;
		MoveHistory *history;
		std::string status;
		std::mt19937 rng;
		int scrambleIndex;
		std::vector<MoveEntry> scramble;
};

#endif // control.h
