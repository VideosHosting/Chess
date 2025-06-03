#include "move.h"
#include "board.h"

// for highlighting moves
static MoveList_t legal_moves;


// this makes life so much easier
#define AllocMem(size) (Move_t*)malloc(size * sizeof(Move_t));
#define CheckType(piece, Type, msg) if((piece)->type != Type) { \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", msg); \
    return (MoveList_t){NULL, 0}; \
}
#define Check(moves) if(!moves) { \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Memory allocation failed for moves."); \
    return (MoveList_t){NULL, 0}; \
}
#define ReAllocAttempt(movelist) Move_t* tmp = realloc(movelist.moves, movelist.size * sizeof(Move_t)); \
if(!tmp) {  /* if allocation failed then we just keep it as it is. */ \
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Memory reallocation failed for moves"); \
    return movelist; \
} \
movelist.moves = tmp;

// I'm actually abusing macros 😭🙏🏽🙏🏽🙏🏽


static Move_t* copy(MoveList_t* movelist) {
    Move_t* copy = AllocMem(movelist->size)
    if(copy == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to allocate memory for move copy.");
        return NULL;
    }

    SDL_memcpy(copy, movelist->moves, movelist->size * sizeof(Move_t));
    return copy;
}

void set_legal_moves(MoveList_t movelist) {
    if(legal_moves.size != 0 && legal_moves.moves) {
        free(legal_moves.moves);
        legal_moves.moves = NULL;
        legal_moves.size = 0;
    }
    if(movelist.size == 0) {
        return;
    }

    legal_moves.moves = copy(&movelist);
    if (legal_moves.moves == NULL) {
        legal_moves.size = 0;
        return;
    }
    legal_moves.size = movelist.size;
}

// we'll use a circle for legal move indication (better than a square)
static void draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}

void draw_legal_moves(SDL_Renderer* renderer) {
    if(legal_moves.moves == NULL || legal_moves.size == 0) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128); // semi-transparent green
    int cx, cy, radius;

    for(size_t i = 0; i < legal_moves.size; i++) {
        cx = legal_moves.moves[i].to_col * COL_SIZE + COL_SIZE / 2;
        cy = legal_moves.moves[i].to_row * ROW_SIZE + ROW_SIZE / 2;

        radius = (COL_SIZE < ROW_SIZE ? COL_SIZE : ROW_SIZE) / 6;

        draw_circle(renderer, cx, cy, radius);
        // SDL_RenderFillRect(renderer, &rect);
    }
}

void InitMoveP(Move_t* move, Piece_t* piece, int to_row, int to_col, bool promotion) {
    return InitMove(move, piece->y, piece->x, to_row, to_col, promotion);
}

void InitMove(Move_t* move, int from_row, int from_col, int to_row, int to_col, bool promotion) {
    move->from_row = from_row;
    move->from_col = from_col;

    move->to_row = to_row;
    move->to_col = to_col;

    move->promotion = promotion;
}

void AddMove(MoveList_t* movelist, Move_t* moves, size_t* size) {
    int new_size = movelist->size + *size;
    Move_t* new_moves = AllocMem(new_size);

    SDL_memcpy(new_moves, movelist->moves, movelist->size * sizeof(Move_t));

    for (int i = 0; i < *size; i++) {
        new_moves[i+movelist->size] = moves[i];
    }

    free(movelist->moves);

    movelist->moves = new_moves;
    movelist->size = new_size;
}

void AddMoveM(MoveList_t* movelist, MoveList_t movelist2) {
    AddMove(movelist, movelist2.moves, movelist2.size);
 }

MoveList_t getLegalMoves(Board_t* board, Piece_t* piece) {
    switch(piece->type) {
        case PAWN:
            return PawnMoves(board, piece);
        case ROOK:
            return RookMoves(board, piece);
        case KNIGHT:
            return KnightMoves(board, piece);
        case BISHOP:
            return BishopMoves(board, piece);
        case QUEEN:
            return QueenMoves(board, piece);
        case KING:
            return KingMoves(board, piece);
        default:
            return (MoveList_t){NULL, 0};
    }
}

static inline bool WithinBounds(int row, int col) {
    return row >= 0 && row < DIM_Y &&
           col >= 0 && col < DIM_X;
}

/*
    making a function for Vertical, Horizontal, and Diagonal moves so I can reuse them
    Its used by 3 pieces: Rooks, Bishops and Queens
*/

// takes in Move_t* ptr assuming it has enough space
static void generateVerticalMoves(Board_t* board, Piece_t* piece, Move_t* moves, int* size) {
    int col = piece->x;

    for (int dir = -1; dir <= 1; dir += 2) { // dir = -1 (up), +1 (down)
        int row = piece->y + dir;

        while (WithinBounds(row, col)) {
            Piece_t* target = getPiece(board, row, col);

            if (target && target->color == piece->color) {
                break; // Blocked by same-color piece
            }

            InitMoveP(&moves[(*size)++], piece, row, col, 0);

            if (target && target->color != piece->color) {
                break; // Capture ends line
            }

            row += dir;
        }
    }
}

// takes in Move_t* ptr assuming it has enough space
static void generateDiagonalMoves(Board_t* board, Piece_t* piece, Move_t* moves, int* size) {
    int row, col;
    int directions[4][2] = {
        {-1, -1}, // up-left
        {-1,  1}, // up-right
        { 1, -1}, // down-left
        { 1,  1}  // down-right
    };

    for (int d = 0; d < 4; ++d) {
        int row = piece->y + directions[d][0];
        int col = piece->x + directions[d][1];

        while (WithinBounds(row, col)) {
            Piece_t* target = getPiece(board, row, col);

            if (target && target->color == piece->color) {
                break;
            }

            InitMoveP(&moves[(*size)++], piece, row, col, 0);

            if (target && target->color != piece->color) {
                break;
            }

            row += directions[d][0];
            col += directions[d][1];
        }
    }
}

// takes in Move_t* ptr assuming it has enough space
static void generateHorizontalMoves(Board_t* board, Piece_t* piece, Move_t* moves, int* size) {
    int row = piece->y;
    for(int dir = -1; dir <= 1; dir += 2) {
        int col = piece->x + dir;

        while (WithinBounds(row, col)) {
            Piece_t* target = getPiece(board, row, col);

            if(target && target->color == piece->color) {
                break;
            }

            InitMoveP(&moves[(*size)++], piece, row, col, 0);

            if(target && target->color != piece->color) {
                break;
            }

            col += dir;
        }
    }
}

MoveList_t KingMoves(Board_t* board, Piece_t* piece) {
    CheckType(piece, KING, "Piece is not a Bishop")

    MoveList_t movelist;
    movelist.size = 0;

    movelist.moves = AllocMem(MAX_MOVES_KING);
    Check(movelist.moves);


    int directions[8][2] = {
        {-1, -1}, // up-left
        {-1,  0}, // up
        {-1,  1}, // up-right
        { 0, -1}, // left
        { 0,  1}, // right
        { 1, -1}, // down-left
        { 1,  0}, // down
        { 1,  1}  // down-right
    };

    int row = piece->y, col = piece->x;
    for(int i = 0; i < 8; i++) {
        row = piece->y + directions[i][0];
        col = piece->x + directions[i][1];

        if(WithinBounds(row, col)) {
            Piece_t* target = getPiece(board, row, col);

            if(target && target->color == piece->color)
                continue;

            InitMoveP(&movelist.moves[movelist.size++], piece, row, col, 0);
        }
    }

    if (movelist.size == 0) {
        free(movelist.moves);
        return (MoveList_t){NULL, 0};
    }

    ReAllocAttempt(movelist)

    return movelist;
}

MoveList_t QueenMoves(Board_t* board, Piece_t* piece) {
    CheckType(piece, QUEEN, "Piece is not a Bishop")

    MoveList_t movelist;
    movelist.size = 0;

    movelist.moves = AllocMem(MAX_MOVES_QUEEN);
    Check(movelist.moves);

    generateDiagonalMoves(board, piece, movelist.moves, &movelist.size);
    generateHorizontalMoves(board, piece, movelist.moves, &movelist.size);
    generateVerticalMoves(board, piece, movelist.moves, &movelist.size);

    if (movelist.size == 0) {
        free(movelist.moves);
        return (MoveList_t){NULL, 0};
    }

    ReAllocAttempt(movelist);

    return movelist;
}

MoveList_t BishopMoves(Board_t* board, Piece_t* piece) {
    CheckType(piece, BISHOP, "Piece is not a Bishop")

    MoveList_t movelist;
    movelist.size = 0;
    movelist.moves = AllocMem(MAX_MOVES_BISHOP);

    Check(movelist.moves);

    // *size = 0;
    // Move_t* moves = AllocMem(MAX_MOVES_BISHOP);
    // Check(moves);

    generateDiagonalMoves(board, piece, movelist.moves, &movelist.size);

    if (movelist.size == 0) {
        free(movelist.moves);
        return (MoveList_t){NULL, 0};
    }

    ReAllocAttempt(movelist);

    return movelist;
}

MoveList_t RookMoves(Board_t* board, Piece_t* piece) {
    CheckType(piece, ROOK, "Piece is not a rook")

    MoveList_t movelist;
    movelist.size = 0;
    movelist.moves = AllocMem(MAX_MOVES_ROOK);
    Check(movelist.moves);

    generateVerticalMoves(board, piece, movelist.moves, &movelist.size);
    generateHorizontalMoves(board, piece, movelist.moves, &movelist.size);

    if (movelist.size == 0) {
        free(movelist.moves);
        return (MoveList_t){NULL, 0};
    }

    ReAllocAttempt(movelist);

    return movelist;
}

MoveList_t KnightMoves(Board_t* board, Piece_t* piece) {
    CheckType(piece, KNIGHT, "Piece is not a knight")

    MoveList_t movelist;
    movelist.size = 0;

    movelist.moves = AllocMem(MAX_MOVES_KNIGHT);
    Check(movelist.moves);

    // *size = 0;
    // Move_t* moves = AllocMem(MAX_MOVES_KNIGHT);
    // Check(moves);

    int row = piece->y; // row is y
    int col = piece->x; // col is x

    int offsets[8][2] = {
        {-2, -1}, {-2, +1},
        {-1, -2}, {-1, +2},
        {+1, -2}, {+1, +2},
        {+2, -1}, {+2, +1}
    };

    for (int i = 0; i < 8; i++) {
        int nrow = row + offsets[i][0];
        int ncol = col + offsets[i][1];
        if (WithinBounds(nrow, ncol)) {
            Piece_t* target = getPiece(board, nrow, ncol);

            if (!target || target->color != piece->color) {
                InitMoveP(&movelist.moves[movelist.size++], piece, nrow, ncol, 0);
            }
        }
    }

    if (movelist.size == 0) {
        free(movelist.moves);
        return (MoveList_t){NULL, 0};
    }

    ReAllocAttempt(movelist);

    return movelist;
}

MoveList_t PawnMoves(Board_t* board, Piece_t* piece) {
    CheckType(piece, PAWN, "Piece is not a pawn")

    MoveList_t movelist;
    movelist.size = 0;

    movelist.moves = AllocMem(MAX_MOVES_PAWN);
    Check(movelist.moves);

    int direction = (piece->color == WHITE) ? -1 : 1;
    int start_row = (piece->color == WHITE) ? 6 : 1;
    // *size = 0;

    // Move_t* moves = AllocMem(MAX_MOVES_PAWN);
    // Check(moves);

    // forward
    if(piece->y + direction >= 0 && piece->y + direction < DIM_Y &&
        !getPiece(board, piece->y + direction, piece->x)) {

        InitMoveP(&movelist.moves[movelist.size++], piece, piece->y + direction, piece->x, 0);


        if(piece->y == start_row && 
           !getPiece(board, piece->y + 2 * direction, piece->x)) {
            InitMoveP(&movelist.moves[movelist.size++], piece, piece->y + 2 * direction, piece->x, 0);
        }
    }

    // Capture left
    if(piece->x > 0 && piece->y + direction >= 0 && piece->y + direction < DIM_Y) {
        Piece_t* target = getPiece(board, piece->y + direction, piece->x - 1);
        if(target && target->color != piece->color) {
            InitMoveP(&movelist.moves[movelist.size++], piece, piece->y + direction, piece->x - 1, 0);
        }
    }

    // Capture right
    if(piece->x < DIM_X - 1 && piece->y + direction >= 0 && piece->y + direction < DIM_Y) {
        Piece_t* target = getPiece(board, piece->y + direction, piece->x + 1);
        if(target && target->color != piece->color) {
            InitMoveP(&movelist.moves[movelist.size++], piece, piece->y + direction, piece->x + 1, 0);
        }
    }

    // If no moves found, free allocated memory and return NULL
    if(movelist.size == 0) {
        free(movelist.moves);
        return (MoveList_t){NULL, 0};
    }

    ReAllocAttempt(movelist);

    return movelist;
}}