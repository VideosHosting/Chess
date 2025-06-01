#include "move.h"
#include "board.h"

static Move_t* legal_moves = NULL;
static size_t legal_moves_size = 0;

Move_t* copy(Move_t* moves, size_t size) {
    Move_t* copy = malloc(size * sizeof(Move_t));
    if(copy == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to allocate memory for move copy.");
        return NULL;
    }
    SDL_memcpy(copy, moves, size * sizeof(Move_t));
    return copy;
}

void set_legal_moves(Move_t* moves, size_t size) {
    if(legal_moves_size != 0 && legal_moves) {
        free(legal_moves);
        legal_moves = NULL;
        legal_moves_size = 0;
    }
    if(size == 0) {
        return;
    }

    legal_moves = copy(moves, size);
    legal_moves_size = size;
}

void draw_legal_moves(SDL_Renderer* renderer) {
    if(legal_moves == NULL || legal_moves_size == 0) return;

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 128); // semi-transparent green
    for(size_t i = 0; i < legal_moves_size; i++) {
        SDL_Rect rect;

        rect.x = legal_moves[i].to_col * COL_SIZE;
        rect.y = legal_moves[i].to_row * ROW_SIZE;
        rect.w = COL_SIZE;
        rect.h = ROW_SIZE;

        SDL_RenderFillRect(renderer, &rect);
    }
}

void InitMoveP(Move_t* moves, Piece_t* piece, int to_row, int to_col, bool promotion) {
    moves->from_row = piece->y;
    moves->from_col = piece->x;

    moves->to_row = to_row;
    moves->to_col = to_col;

    moves->promotion = promotion;
}

void InitMove(Move_t* moves, int from_row, int from_col, int to_row, int to_col, bool promotion) {
    moves->from_row = from_row;
    moves->from_col = from_col;

    moves->to_row = to_row;
    moves->to_col = to_col;

    moves->promotion = promotion;
}

Move_t* getLegalMoves(Board_t* board, Piece_t* piece, int* size) {
    switch(piece->type) {
        case PAWN:
            return PawnMoves(board, piece, size);
        case ROOK:
            return RookMoves(board, piece, size);
        case KNIGHT:
            return KnightMoves(board, piece, size);
        case BISHOP:
            return BishopMoves(board, piece, size);
        case QUEEN:
            return QueenMoves(board, piece, size);
        case KING:
            return KingMoves(board, piece, size);
        default:
            return NULL;
    }
}

// this makes life so much easier
#define AllocMem(size) (Move_t*)malloc(size * sizeof(Move_t));
#define Check(moves) if(!moves) { \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Memory allocation failed for moves."); \
    return NULL; \
}

static bool WithinBounds(int row, int col) {
    return row >= 0 && row < DIM_X &&
           col >= 0 && col < DIM_Y;
}


Move_t* KingMoves(Board_t* board, Piece_t* piece, int* size) {
    return NULL;
}
Move_t* QueenMoves(Board_t* board, Piece_t* piece, int* size) {
    return NULL;
}
Move_t* BishopMoves(Board_t* board, Piece_t* piece, int* size) {
    return NULL;
}
Move_t* RookMoves(Board_t* board, Piece_t* piece, int* size) {
    return NULL;
}
Move_t* KnightMoves(Board_t* board, Piece_t* piece, int* size) {
    if (piece->type != KNIGHT) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Piece is not a knight.");
        return NULL;
    }

    *size = 0;
    Move_t* moves = AllocMem(8);
    Check(moves);

    int row = piece->x, col = piece->y;
    row -= 2;

    InitMoveP(&moves[(*size)++], piece, row, col-1, 0);
    InitMoveP(&moves[(*size)++], piece, row, col+1, 0);

    // Move_t* moves = (Move_t*)malloc(8 * sizeof(Move_t));
    // if(!moves) {
    //     SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Memory allocation failed for moves.");
    //     return NULL;
    // }

    return moves;
}

Move_t* PawnMoves(Board_t* board, Piece_t* piece, int* size) {
    if(piece->type != PAWN) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Piece is not a pawn.");
        return NULL;
    }

    int direction = (piece->color == WHITE) ? -1 : 1;
    int start_row = (piece->color == WHITE) ? 6 : 1;
    *size = 0;

    Move_t* moves = AllocMem(4);
    Check(moves);

    // forward
    if(piece->y + direction >= 0 && piece->y + direction < DIM_Y &&
        !getPiece(board, piece->y + direction, piece->x)) {

        InitMoveP(&moves[(*size)++], piece, piece->y + direction, piece->x, 0);


        if(piece->y == start_row && 
           !getPiece(board, piece->y + 2 * direction, piece->x)) {
            InitMoveP(&moves[(*size)++], piece, piece->y + 2 * direction, piece->x, 0);
        }
    }

    // Capture left
    if(piece->x > 0 && piece->y + direction >= 0 && piece->y + direction < DIM_Y) {
        Piece_t* target = getPiece(board, piece->y + direction, piece->x - 1);
        if(target && target->color != piece->color) {
            InitMoveP(&moves[(*size)++], piece, piece->y + direction, piece->x - 1, 0);
        }
    }

    // Capture right
    if(piece->x < DIM_X - 1 && piece->y + direction >= 0 && piece->y + direction < DIM_Y) {
        Piece_t* target = getPiece(board, piece->y + direction, piece->x + 1);
        if(target && target->color != piece->color) {
            InitMoveP(&moves[(*size)++], piece, piece->y + direction, piece->x + 1, 0);
        }
    }

    // If no moves found, free allocated memory and return NULL
    if(*size == 0) {
        free(moves);
        return NULL;
    }

    moves = realloc(moves, (*size) * sizeof(Move_t));
    if(!moves) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Memory reallocation failed for moves.");
        return NULL;
    }
}

// Move_t* PawnMoves(Board_t* board, Piece_t* piece, int* size) {
//     if(piece->type != PAWN) {
//         SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Piece is not a pawn.");
//         return NULL;
//     }

//     int direction = (piece->color == WHITE) ? -1 : 1; // White moves up, Black moves down
//     int start_row = (piece->color == WHITE) ? 6 : 1; // Starting row for pawns

//     Move_t* moves = malloc(4 * sizeof(Move_t)); // Allocate space for up to 4 moves
//     if(!moves) {
//         SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Memory allocation failed for moves.");
//         return NULL;
//     }

//     *size = 0;

//     // Forward move
//     if(piece->y + direction >= 0 && piece->y + direction < DIM_Y && 
//        !getPiece(board, piece->x, piece->y + direction)) {
//         moves[(*size)++] = (Move_t){piece->y, piece->x, piece->x, piece->y + direction, 0};
    
//         // Double forward move from starting position
//         if(piece->y == start_row && 
//            !getPiece(board, piece->x, piece->y + 2 * direction)) {
//             moves[(*size)++] = (Move_t){piece->y, piece->x, piece->x, piece->y + 2 * direction, 0};
//         }
//     }

//     // Capture left
//     if(piece->x > 0 && piece->y + direction >= 0 && piece->y + direction < DIM_Y) {
//         Piece_t* target = getPiece(board, piece->x - 1, piece->y + direction);
//         if(target && target->color != piece->color) {
//             moves[(*size)++] = (Move_t){piece->y, piece->x, piece->x - 1, piece->y + direction, 0};
//         }
//     }

//     // Capture right
//     if(piece->x < DIM_X - 1 && piece->y + direction >= 0 && piece->y + direction < DIM_Y) {
//         Piece_t* target = getPiece(board, piece->x + 1, piece->y + direction);
//         if(target && target->color != piece->color) {
//             moves[(*size)++] = (Move_t){piece->y, piece->x, piece->x + 1, piece->y + direction, 0};
//         }
//     }

//     // If no moves found, free allocated memory and return NULL
//     if(*size == 0) {
//         free(moves);
//         return NULL;
//     }

//     moves = realloc(moves, (*size) * sizeof(Move_t));
//     if(!moves) {
//         SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Memory reallocation failed for moves.");
//         return NULL;
//     }

//     return moves;
// }