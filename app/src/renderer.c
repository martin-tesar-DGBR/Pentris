#include "renderer.h"

#include <string.h>

#include "pentris.h"
#include "hal/pru_shared.h"

//number of piece previews
#define PREVIEW_LENGTH 5
//position where to start drawing the pieces in the queue
//(in this case 1 pixel away from the board, or 11 pixels from the left side of the display)
#define PREVIEW_HORIZONTAL_OFFSET (BOARD_WIDTH + 1)

//COLOUR LAYOUT: 8 bits in RRRGGGBB
//3 bits for red, 3 bits for green, 2 bits for blue (this should be documented in the display instead of here)
//think of it as brightness out of 8 for red/green and 4 for blue
//
//ex. 0x45 = 0b01000101
//red = 0b010 = 2 => it's 2/8 bright
//green = 0b001 = 1 => it's 1/8 bright
//blue = 0b01 = 1 => it's 1/4 bright
//note you can't get 1/8, 3/8, etc. brightness for blue
uint8_t piece_colours[NUM_PIECES + 2] = {
    0x1F, 0x63, 0xF0, 0x4B, 0xF4, 0x32,
    0x9F, 0xEC, 0x7C, 0xE3, 0xD8, 0xE0,
    0xFC, 0x07, 0xF0, 0x01, 0x50, 0x1C,
    0x00,
    0x00
};
//if the ghost piece colour is too bright I can try modifying the display to allow darker colours
#define GHOST_PIECE_COLOUR 0x29

//ordinarily the display is horizontally arranged like the following:
//         col 0  ...  col 31
// row 0:  xxxxxxxxxxxxxxxxxx
// row 1:  xxxxxxxxxxxxxxxxxx
//  ...
// row 15: xxxxxxxxxxxxxxxxxx
//
// our implementation indexes like this: (USE THIS LAYOUT)
//         col 0  ...  col 15
// row 31: xxxxxxxxxxxxxxxxxx
//  ...
// row 1:  xxxxxxxxxxxxxxxxxx
// row 0:  xxxxxxxxxxxxxxxxxx
static inline void set_pixel(volatile uint8_t *buffer, int row, int col, uint8_t colour) {
    buffer[row + col * DISPLAY_COLS] = colour;
}

void renderer_draw(volatile uint8_t *buffer) {
    for (int i = 0; i < DISPLAY_ROWS * DISPLAY_COLS; i++) {
        buffer[i] = 0;
    }
    const enum PieceName *board = pentris_get_board();
    enum PieceName piece_preview[PREVIEW_LENGTH];
    int num_previews = pentris_get_queue(piece_preview, PREVIEW_LENGTH);
    //draw board
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            set_pixel(buffer, i, j, piece_colours[board[i * BOARD_WIDTH + j]]);
        }
    }

    uint8_t current_piece_data[MAX_PIECE_DATA_SIZE];
    enum PieceName name;
    int pos_x, pos_y;
    int current_piece_size;
    pentris_get_piece(current_piece_data, &name, &pos_x, &pos_y, &current_piece_size);

    //draw ghost piece before the actual piece (UNTESTED PLEASE TEST)
    int ghost_pos_x = pos_x;
    int ghost_pos_y = pos_y;
    while (pentris_is_valid_placement(current_piece_data, current_piece_size, ghost_pos_x, ghost_pos_y)) {
        ghost_pos_y--;
    }
    for (int i = 0; i < current_piece_size; i++) {
        if (ghost_pos_y + i >= BOARD_HEIGHT) {
            continue;
        }
        for (int j = 0; j < current_piece_size; j++) {
            if (current_piece_data[i * current_piece_size + j]) {
                set_pixel(buffer, ghost_pos_y + i, ghost_pos_x + j, GHOST_PIECE_COLOUR);
            }
        }
    }

    //draw current piece over ghost piece
    for (int i = 0; i < current_piece_size; i++) {
        //do not let the piece stick above the 20x10 matrix
        if (pos_y + i >= BOARD_HEIGHT) {
            continue;
        }
        //no board bounds checking, assume the piece is in a valid position
        for (int j = 0; j < current_piece_size; j++) {
            if (current_piece_data[i * current_piece_size + j]) {
                set_pixel(buffer, pos_y + i, pos_x + j, piece_colours[name]);
            }
        }
    }
    
    //draw queue
    for (int preview = 0; preview < num_previews; preview++) {
        enum PieceName current_preview_piece = piece_preview[preview];
        //reuses current_piece_data from earlier (can rewrite for clarity if needed)
        int preview_size = pentris_get_piece_data(current_preview_piece, NORTH, current_piece_data);
        //center the piece as best as possible within the 5 pixels width we have
        int horizontal_offset = PREVIEW_HORIZONTAL_OFFSET + (6 - preview_size) / 2;
        //each piece in its starting position has a max height of 3 so we space out the
        //pieces by 3 + 1 = 4 pixels at a time, and start drawing from the top
        int vertical_offset = 4 * (PREVIEW_LENGTH - 1 - preview) + 1;
        for (int i = 0; i < preview_size; i++) {
            for (int j = 0; j < preview_size; j++) {
                if (current_piece_data[i * preview_size + j]) {
                    set_pixel(buffer, i + vertical_offset, j + horizontal_offset, piece_colours[current_preview_piece]);
                }
            }
        }
    }

    //TODO: rest of UI (can include score.h to get the current score) ()
}