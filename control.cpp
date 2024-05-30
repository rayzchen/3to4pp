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
	}
	if (!renderer->animating) {
        if (checkMiddleGyro(window)) return;
        if (checkDirectionalMove(window)) return;

        if (glfwGetKey(window, GLFW_KEY_SPACE)) {
            // gyro outer layer
            MoveEntry entry;
            entry.type = GYRO_OUTER;
            entry.animLength = 2.0f;
            entry.location = -1 * puzzle->outerSlicePos;
            renderer->scheduleMove(entry);
        }
    }
}

bool PuzzleController::checkMiddleGyro(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_M) || glfwGetKey(window, GLFW_KEY_PERIOD)) {
        int direction = (glfwGetKey(window, GLFW_KEY_M)) ? -1 : 1;
        if (puzzle->canGyroMiddle(direction)) {
            MoveEntry entry;
            entry.type = GYRO_MIDDLE;
            entry.animLength = 1.0f;
            entry.location = direction;
            renderer->scheduleMove(entry);
            return true;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_COMMA)) {
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

bool PuzzleController::checkDirectionKeys(GLFWwindow* window, RotateDirection* direction) {
    bool foundDirection = false;
    for (int i = 0; i < 6; i++) {
        if (glfwGetKey(window, directionKeys[i])) {
            // Move outer layer
            foundDirection = true;
            *direction = (RotateDirection)i;
        }
    }
    return foundDirection;
}

bool PuzzleController::checkDirectionalMove(GLFWwindow* window) {
    CellLocation cell;
    RotateDirection direction;
    if (checkCellKeys(window, &cell)) {
        if (glfwGetKey(window, GLFW_KEY_SPACE)) {
            startGyro(cell);
            return true;
        }

        if (checkDirectionKeys(window, &direction)) {
            if (puzzle->canRotateCell(cell, direction)) {
                startCellMove(cell, direction);
                return true;
            }
        }
    } else if (checkDirectionKeys(window, &direction)) {
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
