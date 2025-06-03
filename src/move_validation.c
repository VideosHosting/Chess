#include "move.h"

/*
    * here's the plan:
    * generate every possible move after each turn.
    * This lets us check king moves (the annoying bastard)
    * this might be less memory efficient but
    * we can sacrifce some memory for performance
*/

static bool inline MoveEqual(Move_t* move1, Move_t* move2) {
    return move1->from_col == move2->from_col &&
           move1->from_row == move2->from_row &&
           move1->to_col == move2->to_col &&
           move1->to_row == move2->to_row;
}

static bool IsInMoves(Move_t* move, MoveList_t moves) {
    for(int i = 0; i < moves.size; i++) {
        Move_t* n_move = moves.moves + i;

        if(MoveEqual(move, n_move)) {
            return true;
        }
    }

    return false;
}

// checks if a move is valid.
// this should only take a single move
bool isValidMove(Board_t* board, Piece_t* piece, Move_t* move) {

    MoveList_t moves = getLegalMoves(board, piece);

    if(IsInMoves(move, moves)) {
        return true;
    }
    return false;
}
