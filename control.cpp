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

int PuzzleController::cellKeys[] = {GLFW_KEY_D, GLFW_KEY_V, GLFW_KEY_F, GLFW_KEY_W,
                                    GLFW_KEY_E, GLFW_KEY_C, GLFW_KEY_S, GLFW_KEY_R};
int PuzzleController::directionKeys[] = {GLFW_KEY_I, GLFW_KEY_K, GLFW_KEY_J,
                                         GLFW_KEY_L, GLFW_KEY_O, GLFW_KEY_U};

PuzzleController::PuzzleController(PuzzleRenderer* renderer) {
	this->renderer = renderer;
	this->puzzle = renderer->puzzle;
    history = new MoveHistory();
}

PuzzleController::~PuzzleController() {
    delete this->history;
}

void PuzzleController::updatePuzzle(GLFWwindow *window, double dt) {
	MoveEntry entry;
	if (renderer->updateAnimations(window, dt, &entry)) {
        switch (entry.type) {
            case TURN: puzzle->rotateCell(entry.cell, entry.direction); break;
            case ROTATE: puzzle->rotatePuzzle(entry.direction); break;
            case GYRO: puzzle->gyroCell(entry.cell); break;
            case GYRO_OUTER: puzzle->gyroOuterSlice(); break;
            case GYRO_MIDDLE: puzzle->gyroMiddleSlice(entry.location); break;
        }
        history->insertMove(entry);
	}
}

bool PuzzleController::checkMiddleGyro(int key) {
    if (key == GLFW_KEY_M || key == GLFW_KEY_PERIOD) {
        int direction = (key == GLFW_KEY_M) ? -1 : 1;
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

bool PuzzleController::checkCellKeys(GLFWwindow* window, CellLocation* cell) {
    bool foundCell = false;
    for (int i = 0; i < 8; i++) {
        if (glfwGetKey(window, cellKeys[i])) {
            foundCell = true;
            *cell = (CellLocation)i;
        }
    }
    return foundCell;
}

bool PuzzleController::checkDirectionKey(int key, RotateDirection* direction) {
    bool foundDirection = false;
    for (int i = 0; i < 6; i++) {
        if (key == directionKeys[i]) {
            // Move outer layer
            foundDirection = true;
            *direction = (RotateDirection)i;
        }
    }
    return foundDirection;
}

bool PuzzleController::checkDirectionalMove(GLFWwindow* window, int key) {
    CellLocation cell;
    RotateDirection direction;
    if (checkCellKeys(window, &cell)) {
        if (key == GLFW_KEY_SPACE) {
            startGyro(cell);
            return true;
        }

        if (checkDirectionKey(key, &direction)) {
            if (puzzle->canRotateCell(cell, direction)) {
                startCellMove(cell, direction);
                return true;
            }
        }
    } else if (checkDirectionKey(key, &direction)) {
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

void PuzzleController::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && !(key == GLFW_KEY_Z || key == GLFW_KEY_Y)) {
        historyStatus.clear();
    } else if (action == GLFW_RELEASE && !renderer->animating) {
        if (checkMiddleGyro(key)) return;
        if (checkDirectionalMove(window, key)) return;

        if (key == GLFW_KEY_SPACE) {
            // gyro outer layer
            MoveEntry entry;
            entry.type = GYRO_OUTER;
            entry.animLength = 2.0f;
            entry.location = -1 * puzzle->outerSlicePos;
            renderer->scheduleMove(entry);
        } else if (key == GLFW_KEY_Z) {
            MoveEntry entry;
            if (history->undoMove(&entry)) {
                renderer->scheduleMove(entry);
                historyStatus = "Undid 1 move!";
            } else {
                historyStatus = "No moves left!";
            }
        } else if (key == GLFW_KEY_Y) {
            MoveEntry entry;
            if (history->redoMove(&entry)) {
                renderer->scheduleMove(entry);
                historyStatus = "Redid 1 move!";
            } else {
                historyStatus = "No moves left!";
            }
        }
    }
    // todo: move other keybinds into this function
}

std::string PuzzleController::getHistoryStatus() {
    return historyStatus;
}

MoveHistory::MoveHistory() {
    turnCount = 0;
    undoing = false;
    redoing = false;
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

void PuzzleController::checkOutline(GLFWwindow *window, Shader *shader) {
    CellLocation cell;
    if (checkCellKeys(window, &cell)) {
        renderer->renderCellOutline(shader, cell);
    }
}
