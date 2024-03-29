#include "renderer.h"

#include <string.h>

#include "pentris.h"
#include "hal/pru_shared.h"

#define PREVIEW_LENGTH 5
#define PREVIEW_HORIZONTAL_OFFSET (BOARD_WIDTH + 1)

uint8_t piece_colours[NUM_PIECES + 2] = {
    0x1F, 0x63, 0xF0, 0x4B, 0xF4, 0x32,
    0x9F, 0xEC, 0x7C, 0xE3, 0xD8, 0xE0,
    0xFC, 0x07, 0xF0, 0x01, 0x50, 0x1C,
    0x00,
    0x00
};

//ordinarily the display is horizontally arranged like the following:
//         col 0  ...  col 31
// row 0:  xxxxxxxxxxxxxxxxxx
// row 1:  xxxxxxxxxxxxxxxxxx
//  ...
// row 15: xxxxxxxxxxxxxxxxxx
//
// our implementation indexes like this:
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
    uint8_t piece_data[MAX_PIECE_DATA_SIZE];
    enum PieceName name;
    int pos_x, pos_y;
    int piece_size;
    pentris_get_piece(piece_data, &name, &pos_x, &pos_y, &piece_size);
    enum PieceName piece_preview[PREVIEW_LENGTH];
    int num_previews = pentris_get_queue(piece_preview, PREVIEW_LENGTH);
    //draw board
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            set_pixel(buffer, i, j, piece_colours[board[i * BOARD_WIDTH + j]]);
        }
    }
    //draw current piece
    for (int i = 0; i < piece_size; i++) {
        if (pos_y + i >= BOARD_HEIGHT) {
            continue;
        }
        for (int j = 0; j < piece_size; j++) {
            if (piece_data[i * piece_size + j]) {
                set_pixel(buffer, pos_y + i, pos_x + j, piece_colours[name]);
            }
        }
    }
    //draw queue
    for (int preview = 0; preview < num_previews; preview++) {
        enum PieceName current_piece = piece_preview[preview];
        int preview_size = pentris_get_piece_data(current_piece, NORTH, piece_data);
        int horizontal_offset;
        if (preview_size == 5) {
            horizontal_offset = PREVIEW_HORIZONTAL_OFFSET;
        }
        else {
            horizontal_offset = PREVIEW_HORIZONTAL_OFFSET + (6 - preview_size) / 2;
        }
        int vertical_offset = 4 * (PREVIEW_LENGTH - 1 - preview) + 1;
        for (int i = 0; i < preview_size; i++) {
            for (int j = 0; j < preview_size; j++) {
                if (piece_data[i * preview_size + j]) {
                    set_pixel(buffer, i + vertical_offset, j + horizontal_offset, piece_colours[current_piece]);
                }
            }
        }
    }
}