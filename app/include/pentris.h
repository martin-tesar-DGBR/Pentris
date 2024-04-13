//module for the game state and logic itself
#ifndef _PENTRIS_H_
#define _PENTRIS_H_

#include <stdint.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

#define MAX_PIECE_DATA_SIZE (5 * 5)

//names taken from https://en.wikipedia.org/wiki/Pentomino
//N is flipped from the version seen on wikipedia
//<name>f denotes the mirror image of the respective version
//used for both representing pieces in the queue and on the board
enum PieceName {
    I,  T,  U,  V,  W,  X,
    F,  L,  N,  P,  Y,  Z,
    Ff, Lf, Nf, Pf, Yf, Zf,
    NUM_PIECES,
    EMPTY_PIECE
};

//pieces spawn in the NORTH orientation
//it's important the enums are in this order in order to make finding
//new orientations after cw or ccw rotation easier (just do modulo arithmetic)
enum Orientation {
    NORTH, EAST, SOUTH, WEST,
    NUM_ORIENTATION
};

typedef struct Piece {
    //location of the bottom left corner of the piece data representation
    int posX;
    int posY;
    enum PieceName name;
    enum Orientation orientation;
} piece_t;

enum PentrisInput {
    P_LEFT, P_RIGHT,
    P_ROTATE_CCW, P_ROTATE_CW,
    P_SOFT_DROP, P_HARD_DROP,
    //P_HOLD (we don't have enough space on the attached breadboard for a third button)
};

void pentris_init();

void pentris_input(enum PentrisInput input);
void pentris_tick(int *num_lines_cleared);

int pentris_is_valid_placement(const uint8_t *piece_data, int size, int x, int y);

const enum PieceName *pentris_get_board();
int pentris_get_piece_data(enum PieceName piece, enum Orientation orientation, uint8_t *dst);
void pentris_get_piece(uint8_t *piece_data, enum PieceName *name, int *pos_x, int *pos_y, int *size);
int pentris_get_queue(enum PieceName *queue, int max_capacity);

int pentris_is_gameover();

#endif