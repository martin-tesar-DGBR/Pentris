//a separate module for keeping track of score
//it felt wrong to put it in pentris.c, and it also feels
//weird to have it in its own file considering how minimal it is
#ifndef _SCORE_H_
#define _SCORE_H_

void score_init();

void score_line_clear(int lines_cleared);
int score_get();

#endif