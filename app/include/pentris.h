#ifndef _PENTRIS_H_
#define _PENTRIS_H_

#include <stdint.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

#define MAX_PIECE_DATA_SIZE (5 * 5)

//names taken from https://en.wikipedia.org/wiki/Pentomino
//N is flipped from the version seen on wikipedia
//<name>f denotes the mirror image of the respective version
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

//dimensions of the respective piece data for the purpose of data storage and rotation
extern const int piece_data_size[NUM_PIECES];

extern const uint8_t pieces[NUM_PIECES][MAX_PIECE_DATA_SIZE];

enum PentrisInput {
    P_LEFT, P_RIGHT,
    P_ROTATE_CCW, P_ROTATE_CW,
    P_SOFT_DROP, P_HARD_DROP,
    //P_HOLD
};

void pentris_init();
void pentris_cleanup();

void pentris_input(enum PentrisInput input);
void pentris_tick();

const enum PieceName *pentris_get_board();
int pentris_get_piece_data(enum PieceName piece, enum Orientation orientation, uint8_t *dst);
void pentris_get_piece(uint8_t *piece, enum PieceName *name, int *pos_x, int *pos_y, int *size);
int pentris_get_queue(enum PieceName *queue, int max_capacity);

#endif