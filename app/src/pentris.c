#include "pentris.h"

#include <string.h>

#include "random.h"

#include "hal/util.h"

#define QUEUE_CAPACITY (6 * NUM_PIECES)
#define RANDOMIZER_90_BAG_LEN (5 * NUM_PIECES)
#define TICKS_PER_GRAVITY 15
//the game extends the board above the visible play area
#define MATRIX_HEIGHT (2 * BOARD_HEIGHT)
#define MATRIX_WIDTH BOARD_WIDTH
#define PIECE_SPAWN_HEIGHT 20

//dimensions for each piece
const int piece_data_size[NUM_PIECES] = {
    5, 3, 3, 3, 3, 3,
    3, 4, 4, 3, 4, 3,
    3, 4, 4, 3, 4, 3
};

//we want to spawn each piece such that they all align to the same row
//e.g. if we spawn all pieces such that their y_pos is 17,
//the I piece would appear to spawn 2 tiles above where the T piece would spawn
//you could technically offset by piece_size - 3 instead of having this array but that seems unintuitive
const int piece_spawn_y_offset[NUM_PIECES] = {
    2, 0, 0, 0, 0, 0,
    0, 1, 1, 0, 1, 0,
    0, 1, 1, 0, 1, 0
};

const uint8_t pieces[NUM_PIECES][MAX_PIECE_DATA_SIZE] = {
    { //I
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    },
    { //T
    1, 1, 1,
    0, 1, 0,
    0, 1, 0,
    },
    { //U
    0, 0, 0,
    1, 0, 1,
    1, 1, 1,
    },
    { //V
    1, 0, 0,
    1, 0, 0,
    1, 1, 1,
    },
    { //W
    1, 0, 0,
    1, 1, 0,
    0, 1, 1,
    },
    { //X
    0, 1, 0,
    1, 1, 1,
    0, 1, 0,
    },

    { //F
    0, 1, 0,
    1, 1, 1,
    0, 0, 1,
    },
    { //L
    0, 0, 0, 0,
    0, 0, 0, 1,
    1, 1, 1, 1,
    0, 0, 0, 0,
    },
    { //N
    0, 0, 0, 0,
    0, 0, 1, 1,
    1, 1, 1, 0,
    0, 0, 0, 0,
    },
    { //P
    0, 0, 0,
    1, 1, 0,
    1, 1, 1,
    },
    { //Y
    0, 0, 0, 0,
    0, 0, 1, 0,
    1, 1, 1, 1,
    0, 0, 0, 0,
    },
    { //Z
    1, 1, 0,
    0, 1, 0,
    0, 1, 1,
    },

    { //Ff
    0, 1, 0,
    1, 1, 1,
    1, 0, 0,
    },
    { //Lf
    0, 0, 0, 0,
    1, 0, 0, 0,
    1, 1, 1, 1,
    0, 0, 0, 0,
    },
    { //Nf
    0, 0, 0, 0,
    1, 1, 0, 0,
    0, 1, 1, 1,
    0, 0, 0, 0,
    },
    { //Pf
    0, 0, 0,
    0, 1, 1,
    1, 1, 1,
    },
    { //Yf
    0, 0, 0, 0,
    0, 1, 0, 0,
    1, 1, 1, 1,
    0, 0, 0, 0,
    },
    { //Zf
    0, 1, 1,
    0, 1, 0,
    1, 1, 0,
    },
};


//board is ordered in this fashion:
// ...
// 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
// 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
//  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
static enum PieceName board[MATRIX_WIDTH * MATRIX_HEIGHT];
static piece_t current_piece;
//information to randomize pieces
uint32_t queue_rng_state;
static enum PieceName queuePieces[NUM_PIECES];
//circular buffer of pieces to place on the board
static enum PieceName queue[QUEUE_CAPACITY];
static int queue_length;
static int queue_head;
static int queue_tail;

static int gravity_accumulator;

#define MOVE_LEFT_MASK 0x01
#define MOVE_RIGHT_MASK 0x02
#define ROTATE_CCW_MASK 0x04
#define ROTATE_CW_MASK 0x08
#define SOFT_DROP_MASK 0x10
#define HARD_DROP_MASK 0x20
static uint8_t input_flags;

static int is_gameover;

//shuffles the 5 * 18 pieces and inserts them into the piece queue
static void refill_queue() {
    //do not refill the queue if doing so will overflow the buffer
    if (queue_length + RANDOMIZER_90_BAG_LEN >= QUEUE_CAPACITY) {
        return;
    }

    queue_rng_state = shuffle(queuePieces, sizeof(*queuePieces), NUM_PIECES, queue_rng_state);
    for (int i = 0; i < RANDOMIZER_90_BAG_LEN; i++) {
        queue[queue_head] = queuePieces[i];
        queue_head = (queue_head + 1) % QUEUE_CAPACITY;
    }
    queue_length += RANDOMIZER_90_BAG_LEN;
}

static void dequeue_piece() {
    current_piece.name = queue[queue_tail];
    queue_tail = (queue_tail + 1) % QUEUE_CAPACITY;
    queue_length -= 1;
    //set x position such that the piece is centered
    //..xxxxx... 5 wide piece
    //...xxxx... 4 wide piece
    //...xxx.... 3 wide piece
    int piece_size = piece_data_size[current_piece.name];
    current_piece.posX = (BOARD_WIDTH - piece_size) / 2;
    current_piece.posY = PIECE_SPAWN_HEIGHT - piece_spawn_y_offset[current_piece.name];
    current_piece.orientation = NORTH;
    //we want at least the number of previews + 1 in the queue at all times (6 was chosen arbitrarily)
    if (queue_length < 6) {
        refill_queue();
    }
}

//can be used to reinitialize the game after it's been initialized once
void pentris_init() {
    is_gameover = 0;
    input_flags = 0;

    for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; i++) {
        board[i] = EMPTY_PIECE;
    }
    
    //for debugging purposes the piece queue is deterministic on the seed, want to change it to use time on release
    queue_rng_state = (uint32_t) get_time_ms();
    //fill queue with 5 of each piece
    for (int i = 0; i < RANDOMIZER_90_BAG_LEN; i++) {
        queuePieces[i] = i % NUM_PIECES;
    }
    queue_length = 0;
    queue_head = 0;
    queue_tail = 0;
    refill_queue();
    dequeue_piece();

    gravity_accumulator = 0;
}

void pentris_input(enum PentrisInput input) {
    switch (input) {
        case P_LEFT:
        input_flags |= MOVE_LEFT_MASK;
        break;
        case P_RIGHT:
        input_flags |= MOVE_RIGHT_MASK;
        break;
        case P_ROTATE_CCW:
        input_flags |= ROTATE_CCW_MASK;
        break;
        case P_ROTATE_CW:
        input_flags |= ROTATE_CW_MASK;
        break;
        case P_SOFT_DROP:
        input_flags |= SOFT_DROP_MASK;
        break;
        case P_HARD_DROP:
        input_flags |= HARD_DROP_MASK;
        break;
    }
}

//checks whether the given piece data with bottom left corner at (x, y) is a valid placement on the board
int pentris_is_valid_placement(const uint8_t *piece_data, int size, int x, int y) {
    int is_valid = 1;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            //placement is invalid if a tile would go out of bounds or would intersect an existing tile on the board
            int tile_posX = x + j;
            int tile_posY = y + i;
            if (piece_data[i * size + j] != 0 &&
                ((tile_posX < 0 || tile_posX >= MATRIX_WIDTH ||
                  tile_posY < 0 || tile_posY >= MATRIX_HEIGHT ||
                  board[tile_posY * MATRIX_WIDTH + tile_posX] != EMPTY_PIECE))
            ) {
                is_valid = 0;
                break;
            }
        }
    }

    return is_valid;
}

static void move_left() {
    uint8_t piece_data[MAX_PIECE_DATA_SIZE];
    int size = pentris_get_piece_data(current_piece.name, current_piece.orientation, piece_data);
    int is_valid = pentris_is_valid_placement(piece_data, size, current_piece.posX - 1, current_piece.posY);

    if (is_valid) {
        current_piece.posX = current_piece.posX - 1;
    }
}

static void move_right() {
    uint8_t piece_data[MAX_PIECE_DATA_SIZE];
    int size = pentris_get_piece_data(current_piece.name, current_piece.orientation, piece_data);
    int is_valid = pentris_is_valid_placement(piece_data, size, current_piece.posX + 1, current_piece.posY);

    if (is_valid) {
        current_piece.posX = current_piece.posX + 1;
    }
}

//returns whether moving down was successful
static int move_down() {
    uint8_t piece_data[MAX_PIECE_DATA_SIZE];
    int size = pentris_get_piece_data(current_piece.name, current_piece.orientation, piece_data);
    int is_valid = pentris_is_valid_placement(piece_data, size, current_piece.posX, current_piece.posY - 1);

    if (is_valid) {
        current_piece.posY = current_piece.posY - 1;
        return 1;
    }
    return 0;
}

//returns the number of cleared lines while clearing lines
static int clear_lines() {
    int full_rows[5];
    int num_full_rows = 0;
    for (int row = 0; row < MATRIX_HEIGHT; row++) {
        int is_full = 1;
        for (int col = 0; col < MATRIX_WIDTH; col++) {
            if (board[row * MATRIX_WIDTH + col] == EMPTY_PIECE) {
                is_full = 0;
                break;
            }
        }
        if (is_full) {
            full_rows[num_full_rows] = row;
            num_full_rows++;
        }
    }

    if (num_full_rows == 0) {
        return 0;
    }

    //copy down each row
    //if row is above a cleared line, we have to copy it down further
    //there's technically a bug where the top n rows are not set to the empty row,
    //but the player is unlikely to reach that area
    int num_rows_copy = 0;
    for (int i = full_rows[num_rows_copy] + 1; i < MATRIX_HEIGHT; i++) {
        if (num_rows_copy < num_full_rows && i > full_rows[num_rows_copy]) {
            num_rows_copy++;
        }
        if (num_rows_copy > 0) {
            memcpy(&board[(i - num_rows_copy) * MATRIX_WIDTH], &board[i * MATRIX_WIDTH], sizeof(*board) * MATRIX_WIDTH);
        }
    }

    return num_full_rows;
}

//place the current piece as it is onto the board, get the next piece from the queue and return the number of lines cleared
//NOTE: can be used while the piece is floating in midair, beware of usage
static void place_piece(uint8_t *piece_data, int size, int *num_lines_cleared) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (piece_data[i * size + j]) {
                board[((current_piece.posY + i) * MATRIX_WIDTH) + (current_piece.posX + j)] = current_piece.name;
            }
        }
    }

    *num_lines_cleared = clear_lines();
    dequeue_piece();
}

//effectively hard drop, moves piece down until it can't be moved anymore and places the piece
static void drop_down_and_place(int *num_lines_cleared) {
    uint8_t piece_data[MAX_PIECE_DATA_SIZE];
    int size = pentris_get_piece_data(current_piece.name, current_piece.orientation, piece_data);
    int is_valid = 1;
    while (is_valid) {
        is_valid = pentris_is_valid_placement(piece_data, size, current_piece.posX, current_piece.posY - 1);
        if (is_valid) {
            current_piece.posY = current_piece.posY - 1;
        }
    }

    place_piece(piece_data, size, num_lines_cleared);
}

//apply one instance of gravity
//one instance of gravity moves the piece down one tile if possible, or places it otherwise
static void apply_gravity(int *num_lines_cleared) {
    uint8_t piece_data[MAX_PIECE_DATA_SIZE];
    int size = pentris_get_piece_data(current_piece.name, current_piece.orientation, piece_data);
    if (pentris_is_valid_placement(piece_data, size, current_piece.posX, current_piece.posY - 1)) {
        current_piece.posY = current_piece.posY - 1;
    }
    else {
        place_piece(piece_data, size, num_lines_cleared);
    }
}

static void rotate_ccw() {
    enum Orientation new_orientation = (current_piece.orientation + 3) % NUM_ORIENTATION;
    uint8_t piece_data[MAX_PIECE_DATA_SIZE];
    int size = pentris_get_piece_data(current_piece.name, new_orientation, piece_data);
    int is_valid = pentris_is_valid_placement(piece_data, size, current_piece.posX, current_piece.posY);

    if (is_valid) {
        current_piece.orientation = new_orientation;
    }
}

static void rotate_cw() {
    enum Orientation new_orientation = (current_piece.orientation + 1) % NUM_ORIENTATION;
    uint8_t piece_data[MAX_PIECE_DATA_SIZE];
    int size = pentris_get_piece_data(current_piece.name, new_orientation, piece_data);
    int is_valid = pentris_is_valid_placement(piece_data, size, current_piece.posX, current_piece.posY);

    if (is_valid) {
        current_piece.orientation = new_orientation;
    }
}

//there's a lot of output arguments but I think the names explain what they do
void pentris_tick(int *num_lines_cleared) {
    //-1 for no value, 0 for piece placed, > 0 for number of lines cleared
    *num_lines_cleared = -1;
    if (is_gameover) {
        return;
    }

    if (((input_flags & MOVE_LEFT_MASK) != 0) && ((input_flags & MOVE_RIGHT_MASK) == 0)) {
        move_left();
    }
    if (((input_flags & MOVE_RIGHT_MASK) != 0) && ((input_flags & MOVE_LEFT_MASK) == 0)) {
        move_right();
    }
    if (((input_flags & ROTATE_CCW_MASK) != 0) && ((input_flags & ROTATE_CW_MASK) == 0)) {
        rotate_ccw();
    }
    if (((input_flags & ROTATE_CW_MASK) != 0) && ((input_flags & ROTATE_CCW_MASK) == 0)) {
        rotate_cw();
    }
    //when soft dropping, we want to reset the gravity accumulator so we don't get double dropping situations
    //however, we don't want to reset it every time we move down or the player can stall at the bottom of the board
    //therefore we only reset the accumulator when the piece was successfully moved down
    if (input_flags & SOFT_DROP_MASK) {
        if (move_down()) {
            gravity_accumulator = 0;
        }
    }

    //I apologize for nested function hell in advance.
    if (input_flags & HARD_DROP_MASK) {
        drop_down_and_place(num_lines_cleared);
        gravity_accumulator = 0;
    }
    else {
        gravity_accumulator++;
        if (gravity_accumulator >= TICKS_PER_GRAVITY) {
            apply_gravity(num_lines_cleared);
            gravity_accumulator = 0;
        }
    }

    //check if the current piece is invalid (by spawning into another piece) and end the game if so
    uint8_t piece_data[MAX_PIECE_DATA_SIZE];
    int piece_size = pentris_get_piece_data(current_piece.name, current_piece.orientation, piece_data);
    if (!pentris_is_valid_placement(piece_data, piece_size, current_piece.posX, current_piece.posY)) {
        is_gameover = 1;
    }

    input_flags = 0;
}

const enum PieceName *pentris_get_board() {
    return board;
}

//returns size of bounding box of piece
int pentris_get_piece_data(enum PieceName piece, enum Orientation orientation, uint8_t *dst) {
    int piece_size = piece_data_size[piece];
    int num_elements = piece_size * piece_size;
    memcpy(dst, &pieces[piece], num_elements);
    //flip vertically since the data is oriented 0 at the top instead of the bottom
    for (int i = 0; i < piece_size / 2; i++) {
        swap(dst + i * piece_size, dst + (piece_size - 1 - i) * piece_size, piece_size);
    }
    switch (orientation) {
        case NORTH:
        //do nothing
        break;
        case EAST:
        //rotate 90 degrees CW
        for (int i = 0; i < (piece_size + 1) / 2; i++) {
            for (int j = i; j < piece_size - 1 - i; j++) {
                int temp = dst[i * piece_size + j];
                dst[i * piece_size + j] = dst[(piece_size - 1 - j) * piece_size + i];
                dst[(piece_size - 1 - j) * piece_size + i] = dst[(piece_size - 1 - i) * piece_size + (piece_size - 1 - j)];
                dst[(piece_size - 1 - i) * piece_size + (piece_size - 1 - j)] = dst[j * piece_size + (piece_size - 1 - i)];
                dst[j * piece_size + (piece_size - 1 - i)] = temp;
            }
        }
        break;
        case SOUTH:
        //rotate 180 degrees
        for (int i = 0; i < (piece_size + 1) / 2; i++) {
            for (int j = i; j < piece_size - 1 - i; j++) {
                int temp = dst[i * piece_size + j];
                dst[i * piece_size + j] = dst[(piece_size - 1 - i) * piece_size + (piece_size - 1 - j)];
                dst[(piece_size - 1 - i) * piece_size + (piece_size - 1 - j)] = temp;

                temp = dst[j * piece_size + (piece_size - 1 - i)];
                dst[j * piece_size + (piece_size - 1 - i)] = dst[(piece_size - 1 - j) * piece_size + i];
                dst[(piece_size - 1 - j) * piece_size + i] = temp;
            }
        }
        break;
        case WEST:
        //rotate 90 degrees CCW
        for (int i = 0; i < (piece_size + 1) / 2; i++) {
            for (int j = i; j < piece_size - 1 - i; j++) {
                int temp = dst[i * piece_size + j];
                dst[i * piece_size + j] = dst[j * piece_size + (piece_size - 1 - i)];
                dst[j * piece_size + (piece_size - 1 - i)] = dst[(piece_size - 1 - i) * piece_size + (piece_size - 1 - j)];
                dst[(piece_size - 1 - i) * piece_size + (piece_size - 1 - j)] = dst[(piece_size - 1 - j) * piece_size + i];
                dst[(piece_size - 1 - j) * piece_size + i] = temp;
            }
        }
        break;
        default:
        break;
    }
    return piece_size;
}

void pentris_get_piece(uint8_t *piece_data, enum PieceName *name, int *pos_x, int *pos_y, int *size) {
    *size = pentris_get_piece_data(current_piece.name, current_piece.orientation, piece_data);
    *name = current_piece.name;
    *pos_x = current_piece.posX;
    *pos_y = current_piece.posY;
}

//request the first max_capacity members of the piece queue
//returns the actual number of pieces read (should always be equal if nothing goes wrong)
int pentris_get_queue(enum PieceName *dst, int max_capacity) {
    int num_read = queue_length < max_capacity ? queue_length : max_capacity;
    int index = queue_tail;
    for (int i = 0; i < num_read; i++) {
        dst[i] = queue[index];
        index = (index + 1) % QUEUE_CAPACITY;
    }
    return num_read;
}

int pentris_is_gameover() {
    return is_gameover;
}