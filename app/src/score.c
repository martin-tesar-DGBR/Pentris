#include "score.h"

#define MAX_LINE_CLEAR 5
//the display is 16 pixels wide, so assuming 3 pixels wide per digit that's
//16 / (3 + 1) = 4 digits max
#define MAX_SCORE 9999
//points per line cleared: inf, 5, 7.5, 10, 15, 20
//0 lines cleared <-> 1 point for placing a piece
static const int line_clear_scores[MAX_LINE_CLEAR + 1] = {1, 5, 15, 30, 60, 100};

static int score = 0;

void score_init() {
    score = 0;
}

void score_line_clear(int lines_cleared) {
    if (lines_cleared < 0 || lines_cleared > MAX_LINE_CLEAR) {
        return;
    }
    score += line_clear_scores[lines_cleared];
}

int score_get() {
    return score;
}