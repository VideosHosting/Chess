#ifndef MOVE_H
#define MOVE_H
#include "piece.h"
#include "setting.h"
#include <stdbool.h>

// circular includes are so annoying
typedef struct Board Board_t;
typedef struct Piece Piece_t;

typedef struct Move {
    int from_row, from_col;
    int to_row, to_col;

    bool promotion;
} Move_t;

void InitMoves(Move_t* moves, int from_row, int from_col, int to_row, int to_col, bool promotion);

Move_t* PawnMoves(Board_t* board, Piece_t* piece, int* size);

void set_legal_moves(Move_t* moves, size_t size);
void draw_legal_moves(SDL_Renderer* renderer);

// Move_t CreateMove(int from_row, int from_col, int to_row, int to_col, char promotion) {
//     Move_t move;
//     move.from_row = from_row;
//     move.from_col = from_col;
//     move.to_row = to_row;
//     move.to_col = to_col;
//     move.promotion = promotion; // Default is no promotion
//     return move;
// }

#endif // MOVE_H

