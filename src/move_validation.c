#include "move.h"
#include "move_internal.h"
#include "board.h"

/*
    * here's the plan:
    * generate every possible move after each turn.
    * This lets us check king moves (the annoying bastard)
    * this might be less memory efficient but
    * we can sacrifce some memory for performance
*/

// will hold 
static MoveList_t LegalMoves;

static bool inline MoveEqual(Move_t* move1, Move_t* move2) {
    return move1->from_col == move2->from_col &&
           move1->from_row == move2->from_row &&
           move1->to_col == move2->to_col &&
           move1->to_row == move2->to_row;
}

static bool IsInMoves(Move_t* move, MoveList_t moves) {
    if(!move || !moves.moves) return false;
    
    for(int i = 0; i < moves.size; i++) {
        Move_t* n_move = moves.moves + i;

        if(MoveEqual(move, n_move)) {
            return true;
        }
    }

    return false;
}

static MoveList_t getPawnAttackMoves(Board_t* board, Piece_t* piece) {
    MoveList_t movelist;
    movelist.size = 0;

    movelist.moves = AllocMem(MAX_MOVES_PAWN);
    Check(movelist.moves);

    int direction = (piece->color == WHITE) ? -1 : 1;
    
    // Capture left
    if(piece->x > 0 && piece->y + direction >= 0 && piece->y + direction < DIM_Y) {
            InitMoveP(&movelist.moves[movelist.size++], piece, piece->y + direction, piece->x - 1, 0);
    }

    // Capture right
    if(piece->x < DIM_X - 1 && piece->y + direction >= 0 && piece->y + direction < DIM_Y) {
            InitMoveP(&movelist.moves[movelist.size++], piece, piece->y + direction, piece->x + 1, 0);
    }

    if (movelist.size == 0) {
        free(movelist.moves);
        return (MoveList_t){NULL, 0};
    }

    return movelist;
}

MoveList_t getAttackMoves(Board_t* board, PieceColor_t color) {
    MoveList_t finalMoves = {NULL, 0};

    for(int row = 0; row < DIM_Y; row++) {
        for(int col = 0; col < DIM_X; col++) {
            Piece_t* target = getPiece(board, row, col);

            // refuse if: empty square, wrong color, or its a king
            if(!target || target->color != color || target->type == KING)
                //TODO: Add pawn attack moves later
                continue;
            
            MoveList_t moves;

            if(target->type == PAWN)
                moves = getPawnAttackMoves(board, target);
            else
                moves = getLegalMoves(board, target);

            AddMoveM(&finalMoves, moves);

            free(moves.moves);
        }
    }

    return finalMoves;
}

bool IsCheck(Board_t* board, PieceColor_t color) {
    if(!board->WhiteKing || !board->BlackKing) {
        ERROR("Both Kings aren't present!");
        return false;
    }

    MoveList_t enemy_attacks = getAttackMoves(board,
                            (color == WHITE) ?BLACK : WHITE);

    Piece_t* king = (color == WHITE) ? board->WhiteKing : board->BlackKing;
    // LOG("King Coords: (%dx%d)", king->y, king->x);
    for (int i = 0; i < enemy_attacks.size; i++) {
        int r = enemy_attacks.moves[i].to_row;
        int c = enemy_attacks.moves[i].to_col;

        // LOG("Checking %dx%d", r, c);
        if (r == king->y && c == king->x) {
            free(enemy_attacks.moves);
            return true; // King is attacked
        }
    }

    free(enemy_attacks.moves);
    return false; // King is safe
}

// checks if a move is valid.
// this should only take a single move
bool isValidMove(Board_t* board, Piece_t* piece, Move_t* move) {
 
     MoveList_t moves = getLegalMoves(board, piece);
    
    bool result = IsInMoves(move, moves);
    
    // Free allocated memory
    if(moves.moves) {
        free(moves.moves);
    }
    
    return result;
 }
