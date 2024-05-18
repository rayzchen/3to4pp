#include "puzzle.h"

Puzzle::Puzzle() {
    // Generate all 2c pieces
    for (int i = 0; i < 7; i++) {
        for (int j = i + 1; j < 8; j++) {
            Color icol = static_cast<Color>(i), jcol = static_cast<Color>(j);
            pieces.push_back({icol, jcol, UNUSED, UNUSED});
            piecemap[{icol, jcol}] = &pieces.back();
        }
    }
    
    // Generate all 3c pieces
    for (int i = 0; i < 6; i++) {
        for (int j = i + 1; j < 7; j++) {
            for (int k = j + 1; k < 8; k++) {
                Color icol = static_cast<Color>(i),
                    jcol = static_cast<Color>(j),
                    kcol = static_cast<Color>(k);
                pieces.push_back({icol, jcol, kcol, UNUSED});
                piecemap[{icol, jcol, kcol}] = &pieces.back();
            }
        }
    }
    
    // Generate all 4c pieces
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 6; j++) {
            for (int k = j + 1; k < 7; k++) {
                for (int l = k + 1; l < 8; l++) {
                    Color icol = static_cast<Color>(i),
                        jcol = static_cast<Color>(j),
                        kcol = static_cast<Color>(k),
                        lcol = static_cast<Color>(l);
                    piecemap[{icol, jcol, kcol, lcol}] = &pieces.back();
                }
            }
        }
    }

    // Assign pieces to cell
    // assignPieces(RIGHT, WHITE, GREEN, PINK, BLUE, PURPLE, YELLOW);
    // assignPieces(LEFT, WHITE, GREEN, PINK, BLUE, PURPLE, YELLOW);
}
