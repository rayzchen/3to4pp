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

#include "control.h"
#include "constants.h"
#include <algorithm>
#include <array>
#include <random>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
// #define RYML_SINGLE_HDR_DEFINE_NOW
// #include <rapidyaml-0.6.0.hpp>

#ifdef _WIN32
#include <windows.h>
void showError(std::string text) {
    HWND wnd = GetActiveWindow();
    MessageBox(wnd, text.c_str(), "Error", 0x2010);
}

// Prevent name collision with enum
#undef IN
#undef OUT
#else
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
void showError(std::string text) {
    EM_ASM(alert(UTF8ToString($0)), text.c_str());
}
#else
#include <iostream>
void showError(std::string text) {
    std::cerr << text << std::endl;
}
#endif
#endif

int PuzzleController::cellKeys[] = {GLFW_KEY_D, GLFW_KEY_V, GLFW_KEY_F, GLFW_KEY_W,
                                    GLFW_KEY_E, GLFW_KEY_C, GLFW_KEY_S, GLFW_KEY_R};
int PuzzleController::directionKeys[] = {GLFW_KEY_I, GLFW_KEY_K, GLFW_KEY_J,
                                         GLFW_KEY_L, GLFW_KEY_O, GLFW_KEY_U};

PuzzleController::PuzzleController(PuzzleRenderer* renderer) {
	this->renderer = renderer;
	this->puzzle = renderer->puzzle;
    history = new MoveHistory();
    std::random_device rd;
    rng.seed(rd());
    scrambleIndex = -1;
}

PuzzleController::~PuzzleController() {
    delete this->history;
}

void PuzzleController::performMove(MoveEntry entry) {
    // todo
}

bool PuzzleController::updatePuzzle(GLFWwindow *window, double dt) {
	MoveEntry entry;
    bool updated = false;
	if (renderer->updateAnimations(window, dt, &entry)) {
        performMove(entry);
        if (scrambleIndex != -1) {
            if (renderer->pendingMoves.size() == 0) {
                scrambleIndex++;
                performScramble();
            }
        } else {
            history->insertMove(entry);
        }
        updated = true;
	}
    return updated || renderer->animating;
}

void PuzzleController::keyCallback(GLFWwindow* window, int key, int action, int mods, bool flip) {
    if (renderer->animating) return;
    if (action == GLFW_PRESS) {
        status.clear();
        if (mods == 0) {
            // todo

            if (key == GLFW_KEY_Z) {
                undoMove();
            } else if (key == GLFW_KEY_Y) {
                redoMove();
            }
        }
    }
}

void PuzzleController::resetPuzzle() {
    puzzle->resetPuzzle();
    scramble.clear();
    history->reset();
    status = "Reset puzzle!";
}

void PuzzleController::undoMove() {
    MoveEntry entry;
    if (history->undoMove(&entry)) {
        renderer->scheduleMove(entry);
        status = "Undid 1 move!";
    } else {
        status = "Error: nothing to undo!";
    }
}

void PuzzleController::redoMove() {
    MoveEntry entry;
    if (history->redoMove(&entry)) {
        renderer->scheduleMove(entry);
        status = "Redid 1 move!";
    } else {
        status = "Error: nothing to redo!";
    }
}

void PuzzleController::scramblePuzzle(int scrambleLength) {
    // todo

    getScrambleTwists();
    scrambleIndex = 0;
    performScramble();
    status = "Scrambled puzzle!";
}

void PuzzleController::performScramble() {
    static float origRenderSpeed;
    if (scrambleIndex == 0) {
        origRenderSpeed = renderer->animationSpeed;
        renderer->animationSpeed = 40.0f;
    }
    if ((size_t)scrambleIndex == scramble.size()) {
        scrambleIndex = -1;
        renderer->animationSpeed = origRenderSpeed;
    } else {
        // todo
    }
}

void PuzzleController::getScrambleTwists() {
    // todo
}

void PuzzleController::openFile(std::string filename) {
    std::ostringstream loadStatus;
    loadStatus << "Loaded log file from " << filename;
    status = loadStatus.str();
}

std::string PuzzleController::getStatus() {
    return status;
}

MoveHistory::MoveHistory() {
    turnCount = 0;
    undoing = false;
    redoing = false;
}

void MoveHistory::reset() {
    turnCount = 0;
    history.clear();
    redoList.clear();
}

void MoveHistory::insertMove(MoveEntry entry) {
    if (undoing) {
        if (entry.type == TURN) {
            turnCount -= 1;
        }
        undoing = false;
    } else if (history.size() && isOpposite(entry, history.back())) {
        redoList.push_back(history.back());
        history.pop_back();
        if (entry.type == TURN) {
            turnCount -= 1;
        }
    } else {
        if (!redoing) {
            redoList.clear();
        } else {
            redoing = false;
        }
        history.push_back(entry);
        if (entry.type == TURN) {
            turnCount += 1;
        }
    }
}

bool MoveHistory::undoMove(MoveEntry *entry) {
    if (!history.size()) {
        return false;
    }
    MoveEntry lastEntry = history.back();
    *entry = getOpposite(lastEntry);
    undoing = true;
    redoList.push_back(lastEntry);
    history.pop_back();
    return true;
}

bool MoveHistory::redoMove(MoveEntry *entry) {
    if (!redoList.size()) {
        return false;
    }
    *entry = redoList.back();
    redoList.pop_back();
    redoing = true;
    return true;
}

bool MoveHistory::canUndo() {
    return history.size() > 0;
}

bool MoveHistory::canRedo() {
    return redoList.size() > 0;
}

int MoveHistory::getTurnCount() {
    return turnCount;
}

bool isOppositeParity(int a, int b) {
    return a / 2 == b / 2 && a % 2 == 1 - b % 2;
}

int getOppositeParity(int a) {
    return a / 2 * 2 + (1 - a % 2);
}

bool MoveHistory::isOpposite(MoveEntry entry1, MoveEntry entry2) {
    if (entry1.type != entry2.type) {
        return false;
    }
    switch (entry1.type) {
        case GYRO:
            return isOppositeParity((int)entry1.cell, (int)entry2.cell);
        case TURN:
            return entry1.cell == entry2.cell && isOppositeParity((int)entry1.direction, (int)entry2.direction) && entry1.slices == entry2.slices;
        default:
            // should not run
            return false;
    }
}

MoveEntry MoveHistory::getOpposite(MoveEntry entry) {
    MoveEntry opposite;
    opposite.type = entry.type;
    opposite.animLength = entry.animLength;
    switch (entry.type) {
        case GYRO:
            opposite.cell = (CellLocation)getOppositeParity((int)entry.cell);
            break;
        case TURN:
            opposite.cell = entry.cell;
            opposite.direction = (RotateDirection)getOppositeParity((int)entry.direction);
            opposite.slices = entry.slices;
            break;
    }
    return opposite;
}
