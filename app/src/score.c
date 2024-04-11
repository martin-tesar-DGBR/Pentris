#include "score.h"

#define MAX_LINE_CLEAR 5
//the display is 16 pixels wide, so assuming 3 pixels wide per digit that's
//16 / (3 + 1) = 4 digits max
#define MAX_SCORE 10000
//points per line cleared: inf, 15, 22.5, 30, 45, 60
//0 lines cleared <-> 1 point for placing a piece
static const int line_clear_scores[MAX_LINE_CLEAR + 1] = {1, 15, 45, 90, 180, 300};

static int score = 0;
static int combo = 0;

void score_init() {
    score = 0;
    combo = 0;
}

void score_line_clear(int lines_cleared) {
    if (lines_cleared < 0 || lines_cleared > MAX_LINE_CLEAR) {
        return;
    }
    if (lines_cleared > 0) {
        combo += 1;
    }
    else {
        combo = 0;
    }
    score += line_clear_scores[lines_cleared] * ((combo + 2) / 2);
    score = score % MAX_SCORE;
}

int score_get() {
    return score;
}