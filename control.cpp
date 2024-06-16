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
#define RYML_SINGLE_HDR_DEFINE_NOW
#include <rapidyaml-0.6.0.hpp>

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

    std::ifstream file("scramble.txt");
    if (!file.fail()) {
        file >> std::ws;
        std::vector<std::array<int, 2>> moves;
        std::array<int, 2> move;
        char comma;
        while (true) {
            file >> move[0] >> comma >> move[1];
            if (file.eof()) {
                break;
            }
            if (comma == ',') {
                moves.push_back(move);
            }
        }
        for (size_t i = 0; i < moves.size(); i++) {
            CellLocation cell = (CellLocation)moves[i][0];
            if (moves[i][1] == -1) {
                startGyro(cell);
            } else {
                startCellMove(cell, (RotateDirection)moves[i][1]);
            }
            while (renderer->pendingMoves.size()) {
                MoveEntry entry = renderer->pendingMoves.front();
                renderer->pendingMoves.pop();
                performMove(entry);
                scramble.push_back(entry);
            }
        }
        renderer->animating = false;
        getScrambleTwists();
    }
}

PuzzleController::~PuzzleController() {
    delete this->history;
}

void PuzzleController::performMove(MoveEntry entry) {
    switch (entry.type) {
        case TURN: puzzle->rotateCell(entry.cell, entry.direction); break;
        case ROTATE: puzzle->rotatePuzzle(entry.direction); break;
        case GYRO: puzzle->gyroCell(entry.cell); break;
        case GYRO_OUTER: puzzle->gyroOuterSlice(); break;
        case GYRO_MIDDLE: puzzle->gyroMiddleSlice(entry.location); break;
    }
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

bool PuzzleController::checkMiddleGyro(int key, bool flip) {
    if (key == GLFW_KEY_M || key == GLFW_KEY_PERIOD) {
        int direction = (key == GLFW_KEY_M) ? -1 : 1;
        if (flip) direction *= -1;
        if (puzzle->canGyroMiddle(direction)) {
            MoveEntry entry;
            entry.type = GYRO_MIDDLE;
            entry.animLength = 1.0f;
            entry.location = direction;
            renderer->scheduleMove(entry);
            return true;
        }
    } else if (key == GLFW_KEY_COMMA) {
        MoveEntry entry;
        entry.type = GYRO_MIDDLE;
        entry.animLength = 1.0f;
        entry.location = 0;
        renderer->scheduleMove(entry);
        return true;
    }
    return false;
}

bool PuzzleController::checkCellKeys(GLFWwindow* window, CellLocation* cell, bool flip) {
    bool foundCell = false;
    for (int i = 0; i < 8; i++) {
        if (glfwGetKey(window, cellKeys[i])) {
            foundCell = true;
            if (flip && i != 0 && i != 1 && i != 4 && i != 5) {
                // Do not flip IN, OUT, UP, DOWN
                *cell = (CellLocation)(i / 2 * 2 + 1 - i % 2);
            } else {
                *cell = (CellLocation)i;
            }
        }
    }
    return foundCell;
}

bool PuzzleController::checkDirectionKey(int key, RotateDirection* direction, bool flip) {
    bool foundDirection = false;
    for (int i = 0; i < 6; i++) {
        if (key == directionKeys[i]) {
            // Move outer layer
            foundDirection = true;
            if (flip && i != 2 && i != 3) {
                // Do not flip XZ or ZX
                *direction = (RotateDirection)(i / 2 * 2 + 1 - i % 2);
            } else {
                *direction = (RotateDirection)i;
            }
        }
    }
    return foundDirection;
}

bool PuzzleController::checkDirectionalMove(GLFWwindow* window, int key, bool flip) {
    CellLocation cell;
    RotateDirection direction;
    if (checkCellKeys(window, &cell, flip)) {
        if (key == GLFW_KEY_SPACE) {
            startGyro(cell);
            return true;
        }

        if (checkDirectionKey(key, &direction, flip)) {
            if (puzzle->canRotateCell(cell, direction)) {
                startCellMove(cell, direction);
                return true;
            }
        }
    } else if (checkDirectionKey(key, &direction, flip)) {
        if (puzzle->canRotatePuzzle(direction)) {
            // whole puzzle rotation
            MoveEntry entry;
            entry.type = ROTATE;
            entry.animLength = 1.0f;
            entry.direction = direction;
            renderer->scheduleMove(entry);
            return true;
        }
    }
    return false;
}

void PuzzleController::startGyro(CellLocation cell) {
    MoveEntry entry;
    int direction = 0;
    switch (cell) {
        case LEFT:
        case RIGHT:
            entry.type = GYRO;
            entry.animLength = 4.0f;
            entry.cell = cell;
            renderer->scheduleMove(entry);
            break;
        case UP:
        case DOWN:
            if (puzzle->middleSliceDir == FRONT) {
                entry.type = GYRO_MIDDLE;
                entry.animLength = 1.0f;
                entry.location = 0;
                renderer->scheduleMove(entry);
            }

            if (puzzle->middleSlicePos == 0) {
                direction = puzzle->outerSlicePos;
            } else if (puzzle->middleSlicePos == 2 * puzzle->outerSlicePos) {
                direction = -puzzle->outerSlicePos;
            } else if (puzzle->middleSlicePos == -puzzle->outerSlicePos) {
                entry.type = GYRO_OUTER;
                entry.animLength = 2.0f;
                entry.location = -1 * puzzle->outerSlicePos;
                renderer->scheduleMove(entry);
                direction = 0;
            } else if (puzzle->middleSlicePos == puzzle->outerSlicePos) {
                direction = 0;
            }

            if (direction != 0) {
                entry.type = GYRO_MIDDLE;
                entry.animLength = 1.0f;
                entry.location = direction;
                renderer->scheduleMove(entry);
            }

            entry.type = GYRO;
            entry.animLength = 3.0f;
            entry.cell = cell;
            renderer->scheduleMove(entry);
            break;
        case FRONT:
        case BACK:
            if (puzzle->middleSliceDir == UP) {
                entry.type = GYRO_MIDDLE;
                entry.animLength = 1.0f;
                entry.location = 0;
                renderer->scheduleMove(entry);
            }

            if (puzzle->middleSlicePos == 0) {
                direction = puzzle->outerSlicePos;
            } else if (puzzle->middleSlicePos == 2 * puzzle->outerSlicePos) {
                direction = -puzzle->outerSlicePos;
            } else if (puzzle->middleSlicePos == -puzzle->outerSlicePos) {
                entry.type = GYRO_OUTER;
                entry.animLength = 2.0f;
                entry.location = -1 * puzzle->outerSlicePos;
                renderer->scheduleMove(entry);
                direction = 0;
            } else if (puzzle->middleSlicePos == puzzle->outerSlicePos) {
                direction = 0;
            }

            if (direction != 0) {
                entry.type = GYRO_MIDDLE;
                entry.animLength = 1.0f;
                entry.location = direction;
                renderer->scheduleMove(entry);
            }

            entry.type = GYRO;
            entry.animLength = 3.0f;
            entry.cell = cell;
            renderer->scheduleMove(entry);
            break;
        case IN:
        case OUT:
            return;
    }
}

void PuzzleController::startCellMove(CellLocation cell, RotateDirection direction) {
    MoveEntry entry;
    if ((cell == UP || cell == DOWN) && puzzle->middleSliceDir == FRONT) {
        entry.type = GYRO_MIDDLE;
        entry.animLength = 1.0f;
        entry.location = 0;
        renderer->scheduleMove(entry);
    } else if ((cell == FRONT || cell == BACK) && puzzle->middleSliceDir == UP) {
        entry.type = GYRO_MIDDLE;
        entry.animLength = 1.0f;
        entry.location = 0;
        renderer->scheduleMove(entry);
    }

    float length;
    if (cell == UP || cell == DOWN || cell == FRONT || cell == BACK) {
        length = 2.0f;
    } else {
        length = 1.0f;
    }

    entry.type = TURN;
    entry.animLength = length;
    entry.cell = cell;
    entry.direction = direction;
    renderer->scheduleMove(entry);
}

void PuzzleController::keyCallback(GLFWwindow* window, int key, int action, int mods, bool flip) {
    if (renderer->animating) return;
    if (action == GLFW_PRESS) {
        status.clear();
        if (mods == 0) {
            if (checkMiddleGyro(key, flip)) return;
            if (checkDirectionalMove(window, key, flip)) return;

            if (key == GLFW_KEY_SPACE) {
                // gyro outer layer
                MoveEntry entry;
                entry.type = GYRO_OUTER;
                entry.animLength = 2.0f;
                entry.location = -1 * puzzle->outerSlicePos;
                renderer->scheduleMove(entry);
            } else if (key == GLFW_KEY_Z) {
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

void rotate4in8(std::array<int, 8>& cells, std::array<int, 4> indices) {
    int temp = cells[indices[0]];
    for (int i = 0; i < 3; i++) {
        cells[indices[i]] = cells[indices[i + 1]];
    }
    cells[indices[3]] = temp;
}

void rotate8bycell(std::array<int, 8>& cells, CellLocation cell) {
    if (cell == RIGHT) {
        rotate4in8(cells, {0, 6, 1, 7});
    } else if (cell == LEFT) {
        rotate4in8(cells, {1, 6, 0, 7});
    } else if (cell == UP) {
        rotate4in8(cells, {2, 6, 3, 7});
    } else if (cell == DOWN) {
        rotate4in8(cells, {3, 6, 2, 7});
    } else if (cell == FRONT) {
        rotate4in8(cells, {4, 6, 5, 7});
    } else if (cell == BACK) {
        rotate4in8(cells, {5, 6, 4, 7});
    }
}

void PuzzleController::scramblePuzzle(int scrambleLength) {
    static std::discrete_distribution<int> typeDist({2, 3});
    static std::uniform_int_distribution<int> directionDist(0, 5);
    static std::discrete_distribution<int> cellDist({2, 2, 6, 6, 1, 1, 1, 1});
    // FBUD only for visual effect, functionally unneeded
    static std::uniform_int_distribution<int> boolDist(0, 1);
    MoveEntry entry;
    entry.type = TURN;
    if (scrambleLength == 0) {
        scrambleLength = 45;
    }
    CellLocation lastCell = (CellLocation)-1;
    for (int i = 0; i < scrambleLength; i++) {
        if (typeDist(rng) == 0 && entry.type != GYRO) {
            // don't include gyros in scramble length
            i--;
            entry.type = GYRO;
            entry.cell = (CellLocation)(2 + directionDist(rng));
        } else {
            entry.type = TURN;
            while (true) {
                entry.cell = (CellLocation)cellDist(rng);
                if (entry.cell != lastCell) break;
                if (entry.cell == LEFT || entry.cell == RIGHT) break;
            }
            lastCell = entry.cell;
            switch (entry.cell) {
                case IN:
                case OUT:
                    // YZ or ZY
                    entry.direction = (RotateDirection)boolDist(rng);
                    break;
                case UP:
                case DOWN:
                    // XZ or ZX
                    entry.direction = (RotateDirection)(2 + boolDist(rng));
                    break;
                case FRONT:
                case BACK:
                    // XY or YX
                    entry.direction = (RotateDirection)(4 + boolDist(rng));
                    break;
                case LEFT:
                case RIGHT:
                    // any
                    entry.direction = (RotateDirection)directionDist(rng);
                    break;
            }
        }
        scramble.push_back(entry);
    }

    bool reorient = false;
    if (reorient) {
        // R, L, U, D, F, B, O, I
        // Reorient to HSC default orientation
        std::array<int, 8> cells = {0, 1, 4, 5, 3, 2, 6, 7};
        for (size_t i = 0; i < scramble.size(); i++) {
            if (scramble[i].type == GYRO) {
                rotate8bycell(cells, scramble[i].cell);
            }
        }
        entry.type = GYRO;
        std::array<int, 3> colorsToFix = {4, 2, 7};
        for (int i = 0; i < 3; i++) {
            int index = std::distance(cells.begin(), std::find(cells.begin(), cells.end(), colorsToFix[i]));
            if (index != colorsToFix[i]) {
                // Move to I
                if (index == 6) {
                    entry.cell = LEFT;
                    scramble.push_back(entry);
                    scramble.push_back(entry);
                    rotate8bycell(cells, entry.cell);
                    rotate8bycell(cells, entry.cell);
                } else if (index < 6) {
                    entry.cell = (CellLocation)(2 + index);
                    scramble.push_back(entry);
                    rotate8bycell(cells, entry.cell);
                }
                // Move to desired location (gyro opposite)
                if (colorsToFix[i] != 7) {
                    // Don't gyro if inner
                    entry.cell = (CellLocation)(2 + colorsToFix[i] + 1);
                    scramble.push_back(entry);
                    rotate8bycell(cells, entry.cell);
                }
            }
        }
    }

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
        if (scramble[scrambleIndex].type == GYRO) {
            startGyro(scramble[scrambleIndex].cell);
        } else {
            startCellMove(scramble[scrambleIndex].cell, scramble[scrambleIndex].direction);
        }
    }
}

void PuzzleController::getScrambleTwists() {
    std::map<CellLocation, std::pair<int, int>> gyroMoves = {
        {RIGHT, {2, 4}}, // U cell turns F
        {LEFT, {2, 5}}, // U cell turns B
        {UP, {4, 0}}, // F cell turns R
        {DOWN, {4, 1}}, // F cell turns L
        {FRONT, {2, 1}}, // U cell turns R
        {BACK, {2, 0}} // U cell turns L
    };
    std::ostringstream hscScramble;
    hscScramble << 0 << "," << 0 << "," << 7 << " ";
    for (size_t i = 0; i < scramble.size(); i++) {
        if (scramble[i].type == GYRO) {
            hscScramble << gyroMoves[scramble[i].cell].first << "," << gyroMoves[scramble[i].cell].second;
            hscScramble << "," << 7 << " ";
        } else {
            int cell, direction;
            if (scramble[i].cell == IN) {
                cell = 7;
            } else if (scramble[i].cell == OUT) {
                cell = 6;
            } else {
                cell = (int)scramble[i].cell - 2;
            }
            direction = (int)scramble[i].direction;
            if (cell >= 2 && cell < 6) direction += 6;
            hscScramble << cell << "," << direction << "," << 1 << " ";
        }
    }

    std::ostringstream physScramble;
    for (size_t i = 0; i < scramble.size(); i++) {
        if (scramble[i].type == GYRO) {
            physScramble << (int)scramble[i].cell << "," << -1 << " ";
        } else {
            physScramble << scramble[i].cell << "," << (int)scramble[i].direction << " ";
        }
    }
    std::cout << "scramble: >\n  " << hscScramble.str() << std::endl;
    std::cout << "phys_scramble: >\n  " << physScramble.str() << std::endl;
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
            return entry1.cell == entry2.cell && isOppositeParity((int)entry1.direction, (int)entry2.direction);
        case ROTATE:
            return isOppositeParity((int)entry1.direction, (int)entry2.direction);
        case GYRO_OUTER:
            return entry1.location == -entry2.location;
        case GYRO_MIDDLE:
            return entry1.location == -entry2.location;
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
            break;
        case ROTATE:
            opposite.direction = (RotateDirection)getOppositeParity((int)entry.direction);
            break;
        case GYRO_OUTER:
            opposite.location = -entry.location;
            break;
        case GYRO_MIDDLE:
            opposite.location = -entry.location;
            break;
    }
    return opposite;
}

bool PuzzleController::checkOutline(GLFWwindow *window, Shader *shader, bool flip) {
    CellLocation cell;
    if (checkCellKeys(window, &cell, flip)) {
        renderer->renderCellOutline(shader, cell);
        return true;
    }
    return false;
}
