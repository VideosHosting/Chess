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

// hate carrying around a Move_t & size ptr
typedef struct MoveList {
    Move_t* moves;
    int size;

} MoveList_t;

void InitMoveP(Move_t* move, Piece_t* piece, int to_row, int to_col, bool promotion);
void InitMove(Move_t* move, int from_row, int from_col, int to_row, int to_col, bool promotion);

void AddMove(MoveList_t* movelist, Move_t* moves, int* size);
void AddMoveM(MoveList_t* movelist, MoveList_t movelist2);

/* move generation */
MoveList_t getLegalMoves(Board_t* board, Piece_t* piece);

MoveList_t KingMoves(Board_t* board, Piece_t* piece);
MoveList_t QueenMoves(Board_t* board, Piece_t* piece);
MoveList_t BishopMoves(Board_t* board, Piece_t* piece);
MoveList_t RookMoves(Board_t* board, Piece_t* piece);
MoveList_t KnightMoves(Board_t* board, Piece_t* piece);
MoveList_t PawnMoves(Board_t* board, Piece_t* piece);

/* move validation */
bool isValidMove(Board_t* board, Piece_t* piece, Move_t* move);

/* for highlighting */
void set_legal_moves(MoveList_t movelist);
void draw_legal_moves(SDL_Renderer* renderer);

#endif // MOVE_H

